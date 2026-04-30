/*
 * oauth.h
 *
 * Google OAuth 2.0 Authorization Code + PKCE flow for desktop apps.
 *
 * High-level usage:
 *   1. oauth_load_config(&cfg, "config.json") — reads client_id/secret and
 *      (optionally) a previously-saved refresh_token from disk.
 *   2. If !cfg.have_refresh_token: oauth_run_auth_flow(&cfg) — opens the
 *      user's browser, listens on http://127.0.0.1:PORT, exchanges the
 *      auth code for tokens, persists the refresh_token.
 *   3. oauth_get_access_token(&cfg, ...) — refreshes the short-lived
 *      access token using the stored refresh_token.
 */

#ifndef GCAL_OAUTH_H
#define GCAL_OAUTH_H

#include <stdbool.h>
#include <stddef.h>

#define GCAL_TOKEN_MAX        2048
#define GCAL_CLIENT_ID_MAX    256
#define GCAL_CLIENT_SECRET_MAX 128
#define GCAL_PATH_MAX         512

typedef struct
{
    /* Loaded from config.json */
    char client_id[GCAL_CLIENT_ID_MAX];
    char client_secret[GCAL_CLIENT_SECRET_MAX];
    char refresh_token[GCAL_TOKEN_MAX];
    bool have_refresh_token;

    /* Runtime config */
    char ca_bundle_path[GCAL_PATH_MAX];   /* /etc/ssl/certs/ca-certificates.crt */
    char config_path[GCAL_PATH_MAX];      /* used to persist updates */
    int  loopback_port;                    /* e.g. 8765 */
} gcal_oauth_t;

/* Load config (and refresh_token if present) from `path`. */
bool oauth_load_config(gcal_oauth_t* cfg, const char* path);

/* Persist the refresh_token (and the rest of the config) back to disk. */
bool oauth_save_config(const gcal_oauth_t* cfg);

/* Run the interactive auth flow. On success, cfg->refresh_token is set
 * and the config has been written to disk. */
bool oauth_run_auth_flow(gcal_oauth_t* cfg);

/* Refresh the access token. Writes a NUL-terminated token into
 * `access_token_buf`. Returns true on success. */
bool oauth_get_access_token(const gcal_oauth_t* cfg,
                            char* access_token_buf, size_t cap);

#endif /* GCAL_OAUTH_H */
