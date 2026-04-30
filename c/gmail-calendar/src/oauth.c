#include "oauth.h"

#include "http_helper.h"
#include "loopback.h"
#include "util.h"

#include <json.h>

#include <stdio.h>
#include <string.h>

#define AUTH_HOST   "accounts.google.com"
#define AUTH_PATH   "/o/oauth2/v2/auth"
#define TOKEN_HOST  "oauth2.googleapis.com"
#define TOKEN_PATH  "/token"

#define SCOPE       "https://www.googleapis.com/auth/calendar.readonly"

/* ------------------------------------------------------------------------- */
/* Local JSON helpers                                                        */
/* ------------------------------------------------------------------------- */

/* Position the reader on the begin_object of the document root, then on
 * the value of `name`. Returns the same result_t codes as
 * json_reader_find_property. */
static result_t json_root_find(json_reader_t* reader, span_t name,
                               json_token_t* out_value)
{
    result_t r = json_reader_rewind(reader);
    if (failed(r)) return r;
    r = json_reader_next_token(reader);
    if (failed(r)) return r;
    if (reader->token.kind != json_token_begin_object) return unexpected_char;
    return json_reader_find_property(reader, name, out_value);
}

/* Copy a string value at root-level property `name` into a NUL-terminated
 * char buffer of size `cap`. Returns false on any failure. */
static bool json_root_get_string(json_reader_t* reader, span_t name,
                                 char* dest, size_t cap)
{
    json_token_t tok;
    if (failed(json_root_find(reader, name, &tok))) return false;
    span_t out;
    if (failed(json_token_get_string_span(
            &tok,
            span_init((uint8_t*)dest, (uint32_t)(cap - 1)),
            &out)))
    {
        return false;
    }
    dest[span_get_size(out)] = '\0';
    return true;
}

/* ------------------------------------------------------------------------- */
/* Config persistence                                                        */
/* ------------------------------------------------------------------------- */

bool oauth_load_config(gcal_oauth_t* cfg, const char* path)
{
    memset(cfg, 0, sizeof *cfg);
    snprintf(cfg->config_path, sizeof cfg->config_path, "%s", path);

    /* Defaults. */
    snprintf(cfg->ca_bundle_path, sizeof cfg->ca_bundle_path,
             "/etc/ssl/certs/ca-certificates.crt");
    cfg->loopback_port = 8765;

    char buf[8192];
    long n = read_file(path, buf, sizeof buf);
    if (n < 0)
    {
        fprintf(stderr, "oauth: cannot read %s\n", path);
        return false;
    }

    json_reader_t r;
    if (failed(json_reader_init(&r, span_init((uint8_t*)buf, (uint32_t)n), NULL)))
    {
        return false;
    }

    /* Required. */
    if (!json_root_get_string(&r, span_from_str_literal("client_id"),
                              cfg->client_id, sizeof cfg->client_id))
    {
        fprintf(stderr, "oauth: config missing client_id\n");
        return false;
    }
    if (!json_root_get_string(&r, span_from_str_literal("client_secret"),
                              cfg->client_secret, sizeof cfg->client_secret))
    {
        fprintf(stderr, "oauth: config missing client_secret\n");
        return false;
    }

    /* Optional. */
    if (json_root_get_string(&r, span_from_str_literal("refresh_token"),
                             cfg->refresh_token, sizeof cfg->refresh_token))
    {
        cfg->have_refresh_token = (cfg->refresh_token[0] != '\0');
    }
    (void)json_root_get_string(&r, span_from_str_literal("ca_bundle_path"),
                               cfg->ca_bundle_path, sizeof cfg->ca_bundle_path);

    json_token_t tok;
    if (!failed(json_root_find(&r, span_from_str_literal("loopback_port"), &tok))
        && tok.kind == json_token_number)
    {
        int32_t p = 0;
        if (!failed(json_token_get_int32(&tok, &p)) && p > 0 && p < 65536)
        {
            cfg->loopback_port = p;
        }
    }
    return true;
}

bool oauth_save_config(const gcal_oauth_t* cfg)
{
    char buf[8192];
    int n = snprintf(buf, sizeof buf,
        "{\n"
        "  \"client_id\":     \"%s\",\n"
        "  \"client_secret\": \"%s\",\n"
        "  \"refresh_token\": \"%s\",\n"
        "  \"ca_bundle_path\":\"%s\",\n"
        "  \"loopback_port\": %d\n"
        "}\n",
        cfg->client_id, cfg->client_secret, cfg->refresh_token,
        cfg->ca_bundle_path, cfg->loopback_port);
    if (n <= 0 || n >= (int)sizeof buf) return false;
    return write_file_secure(cfg->config_path, buf, (size_t)n);
}

/* ------------------------------------------------------------------------- */
/* PKCE helpers                                                              */
/* ------------------------------------------------------------------------- */

static bool make_pkce(char verifier[64], char challenge[64])
{
    uint8_t raw[32];
    if (!rand_buf(raw, sizeof raw)) return false;
    if (b64url_encode(raw, sizeof raw, verifier, 64) < 0) return false;

    uint8_t hash[32];
    sha256_buf((const uint8_t*)verifier, strlen(verifier), hash);
    if (b64url_encode(hash, sizeof hash, challenge, 64) < 0) return false;
    return true;
}

/* ------------------------------------------------------------------------- */
/* Token endpoint POST                                                       */
/* ------------------------------------------------------------------------- */

static bool token_endpoint_post(const gcal_oauth_t* cfg,
                                const char* body,
                                size_t body_len,
                                const char* extract_field,
                                char* out_buf, size_t out_cap,
                                const char* extract_field2,
                                char* out_buf2, size_t out_cap2)
{
    static uint8_t recv_buf[16 * 1024];

    http_helper_req_t req = {
        .host           = TOKEN_HOST,
        .port           = 443,
        .ca_bundle_path = cfg->ca_bundle_path,
        .method         = "POST",
        .path           = TOKEN_PATH,
        .bearer_token   = NULL,
        .body           = span_init((uint8_t*)body, (uint32_t)body_len),
        .content_type   = "application/x-www-form-urlencoded",
    };
    http_helper_resp_t resp;
    if (http_helper_do(&req, recv_buf, sizeof recv_buf, &resp) != 0)
    {
        return false;
    }
    if (resp.status / 100 != 2)
    {
        fprintf(stderr, "oauth: token endpoint HTTP %d:\n  %.*s\n",
                resp.status,
                (int)span_get_size(resp.body),
                (const char*)span_get_ptr(resp.body));
        return false;
    }

    json_reader_t r;
    if (failed(json_reader_init(&r, resp.body, NULL))) return false;

    if (!json_root_get_string(
            &r,
            span_init((uint8_t*)extract_field, (uint32_t)strlen(extract_field)),
            out_buf, out_cap))
    {
        fprintf(stderr, "oauth: response missing field '%s'\n", extract_field);
        return false;
    }

    if (extract_field2 != NULL && out_buf2 != NULL)
    {
        (void)json_root_get_string(
            &r,
            span_init((uint8_t*)extract_field2, (uint32_t)strlen(extract_field2)),
            out_buf2, out_cap2);
    }
    return true;
}

/* ------------------------------------------------------------------------- */
/* Auth code flow with PKCE                                                  */
/* ------------------------------------------------------------------------- */

bool oauth_run_auth_flow(gcal_oauth_t* cfg)
{
    char verifier[64];
    char challenge[64];
    if (!make_pkce(verifier, challenge))
    {
        fprintf(stderr, "oauth: PKCE generation failed\n");
        return false;
    }

    char redirect_uri[64];
    snprintf(redirect_uri, sizeof redirect_uri,
             "http://127.0.0.1:%d", cfg->loopback_port);

    char redirect_enc[256], scope_enc[256], cid_enc[512];
    if (url_encode(redirect_uri, redirect_enc, sizeof redirect_enc) < 0
        || url_encode(SCOPE, scope_enc, sizeof scope_enc) < 0
        || url_encode(cfg->client_id, cid_enc, sizeof cid_enc) < 0)
    {
        return false;
    }

    char url[1024];
    int n = snprintf(url, sizeof url,
        "https://" AUTH_HOST AUTH_PATH
        "?client_id=%s"
        "&redirect_uri=%s"
        "&response_type=code"
        "&scope=%s"
        "&code_challenge=%s"
        "&code_challenge_method=S256"
        "&access_type=offline"
        "&prompt=consent",
        cid_enc, redirect_enc, scope_enc, challenge);
    if (n <= 0 || n >= (int)sizeof url) return false;

    printf("\nOpening your browser for Google sign-in...\n");
    printf("If it does not open automatically, paste this URL manually:\n\n  %s\n\n", url);
    (void)open_browser(url);

    printf("Waiting for redirect on %s ...\n", redirect_uri);
    char code[1024];
    if (!loopback_wait_for_code(cfg->loopback_port, 180, code, sizeof code))
    {
        return false;
    }
    printf("Got authorization code (length=%zu).\n", strlen(code));

    char code_enc[1024], cs_enc[256];
    if (url_encode(code, code_enc, sizeof code_enc) < 0
        || url_encode(cfg->client_secret, cs_enc, sizeof cs_enc) < 0)
    {
        return false;
    }

    char body[2048];
    int blen = snprintf(body, sizeof body,
        "grant_type=authorization_code"
        "&client_id=%s"
        "&client_secret=%s"
        "&code=%s"
        "&code_verifier=%s"
        "&redirect_uri=%s",
        cid_enc, cs_enc, code_enc, verifier, redirect_enc);
    if (blen <= 0 || blen >= (int)sizeof body) return false;

    char access_token[GCAL_TOKEN_MAX];
    char refresh_token[GCAL_TOKEN_MAX];
    refresh_token[0] = '\0';

    if (!token_endpoint_post(cfg, body, (size_t)blen,
                             "access_token",  access_token, sizeof access_token,
                             "refresh_token", refresh_token, sizeof refresh_token))
    {
        return false;
    }

    if (refresh_token[0] == '\0')
    {
        fprintf(stderr,
            "oauth: Google did not return a refresh_token. This usually means\n"
            "       you previously consented for this client. Revoke access at\n"
            "       https://myaccount.google.com/permissions and re-run --auth.\n");
        return false;
    }

    snprintf(cfg->refresh_token, sizeof cfg->refresh_token, "%s", refresh_token);
    cfg->have_refresh_token = true;

    if (!oauth_save_config(cfg))
    {
        fprintf(stderr, "oauth: failed to save config\n");
        return false;
    }
    printf("Refresh token saved to %s\n", cfg->config_path);
    return true;
}

bool oauth_get_access_token(const gcal_oauth_t* cfg,
                            char* access_token_buf, size_t cap)
{
    if (!cfg->have_refresh_token)
    {
        fprintf(stderr, "oauth: no refresh_token; run --auth first\n");
        return false;
    }
    char cid_enc[512], cs_enc[256], rt_enc[GCAL_TOKEN_MAX + 64];
    if (url_encode(cfg->client_id,     cid_enc, sizeof cid_enc) < 0
        || url_encode(cfg->client_secret, cs_enc,  sizeof cs_enc) < 0
        || url_encode(cfg->refresh_token, rt_enc,  sizeof rt_enc) < 0)
    {
        return false;
    }

    char body[GCAL_TOKEN_MAX + 1024];
    int blen = snprintf(body, sizeof body,
        "grant_type=refresh_token"
        "&client_id=%s"
        "&client_secret=%s"
        "&refresh_token=%s",
        cid_enc, cs_enc, rt_enc);
    if (blen <= 0 || blen >= (int)sizeof body) return false;

    return token_endpoint_post(cfg, body, (size_t)blen,
                               "access_token", access_token_buf, cap,
                               NULL, NULL, 0);
}
