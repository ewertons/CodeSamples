#include "http_helper.h"

#include <stdio.h>
#include <string.h>

#include "common_lib_c.h"

#include <http_request.h>
#include <http_response.h>
#include "http_methods.h"
#include "http_codes.h"
#include "http_versions.h"
#include "http_headers.h"
#include "http_endpoint.h"
#include "http_connection.h"

/* Construct a span_t over a NUL-terminated C string. */
static span_t span_from_cstr(const char* s)
{
    return span_init((uint8_t*)s, (uint32_t)strlen(s));
}

/* Parse a status code span like "200" into an int. */
static int parse_status_code(span_t code)
{
    int v = 0;
    const uint8_t* p = span_get_ptr(code);
    uint32_t n = span_get_size(code);
    for (uint32_t i = 0; i < n; i++)
    {
        if (p[i] < '0' || p[i] > '9') return -1;
        v = v * 10 + (p[i] - '0');
    }
    return v;
}

int http_helper_do(const http_helper_req_t* req,
                   uint8_t* recv_buf, uint32_t recv_buf_size,
                   http_helper_resp_t* resp)
{
    http_endpoint_t endpoint;
    http_endpoint_config_t ec = http_endpoint_get_default_secure_client_config();
    ec.remote.hostname = span_from_cstr(req->host);
    ec.remote.port = req->port;
    ec.tls.trusted_certificate_file = req->ca_bundle_path;

    if (http_endpoint_init(&endpoint, &ec) != ok)
    {
        fprintf(stderr, "http_helper: endpoint_init failed (host=%s)\n", req->host);
        return 1;
    }

    http_connection_t conn;
    if (http_endpoint_connect(&endpoint, &conn) != ok)
    {
        fprintf(stderr, "http_helper: connect failed (host=%s:%d)\n",
                req->host, req->port);
        (void)http_endpoint_deinit(&endpoint);
        return 2;
    }

    /* ---- Build request headers in a fixed-size buffer. ------------------ */
    static uint8_t hdr_buf[1024];
    http_headers_t headers;
    if (http_headers_init(&headers, span_init(hdr_buf, sizeof hdr_buf)) != ok)
    {
        (void)http_connection_close(&conn);
        (void)http_endpoint_deinit(&endpoint);
        return 3;
    }

    (void)http_headers_add(&headers, HTTP_HEADER_HOST, span_from_cstr(req->host));
    (void)http_headers_add(&headers, HTTP_HEADER_USER_AGENT,
                           span_from_str_literal("gcal-c-sample/1.0"));
    (void)http_headers_add(&headers, HTTP_HEADER_ACCEPT,
                           span_from_str_literal("application/json"));
    (void)http_headers_add(&headers, HTTP_HEADER_CONNECTION,
                           span_from_str_literal("close"));

    /* Bearer token. */
    char auth_buf[512];
    if (req->bearer_token != NULL && req->bearer_token[0] != '\0')
    {
        int n = snprintf(auth_buf, sizeof auth_buf, "Bearer %s", req->bearer_token);
        if (n <= 0 || n >= (int)sizeof auth_buf)
        {
            (void)http_connection_close(&conn);
            (void)http_endpoint_deinit(&endpoint);
            return 4;
        }
        (void)http_headers_add(&headers,
                               span_from_str_literal("Authorization"),
                               span_init((uint8_t*)auth_buf, (uint32_t)n));
    }

    /* Body length / content-type. */
    char clen[16];
    uint32_t body_len = span_get_size(req->body);
    if (body_len > 0)
    {
        int n = snprintf(clen, sizeof clen, "%u", body_len);
        (void)http_headers_add(&headers, HTTP_HEADER_CONTENT_LENGTH,
                               span_init((uint8_t*)clen, (uint32_t)n));
        (void)http_headers_add(&headers, HTTP_HEADER_CONTENT_TYPE,
                               span_from_cstr(req->content_type));
    }

    /* ---- Build the request struct and send. ----------------------------- */
    span_t method_span = span_from_cstr(req->method);
    span_t path_span   = span_from_cstr(req->path);

    http_request_t request;
    if (http_request_initialize(&request, method_span, path_span,
                                HTTP_VERSION_1_1, headers) != ok)
    {
        (void)http_connection_close(&conn);
        (void)http_endpoint_deinit(&endpoint);
        return 5;
    }
    if (body_len > 0)
    {
        request.body = req->body;
    }
    else
    {
        request.body = SPAN_EMPTY;
    }

    if (http_connection_send_request(&conn, &request) != ok)
    {
        fprintf(stderr, "http_helper: send_request failed (host=%s)\n", req->host);
        (void)http_connection_close(&conn);
        (void)http_endpoint_deinit(&endpoint);
        return 6;
    }

    /* ---- Receive. ------------------------------------------------------- */
    http_response_t response;
    span_t recv_span = span_init(recv_buf, recv_buf_size);
    if (http_connection_receive_response(&conn, recv_span, &response, NULL) != ok)
    {
        fprintf(stderr, "http_helper: receive_response failed (host=%s)\n", req->host);
        (void)http_connection_close(&conn);
        (void)http_endpoint_deinit(&endpoint);
        return 7;
    }

    resp->status = parse_status_code(response.code);
    resp->body   = response.body;

    (void)http_connection_close(&conn);
    (void)http_endpoint_deinit(&endpoint);
    return 0;
}
