/*
 * http_helper.h
 *
 * Tiny HTTPS request helper sitting on top of http-c's client API.
 * Performs a single request/response cycle (no keep-alive). Returns the
 * full response body as a slice into the caller-supplied recv buffer.
 */

#ifndef GCAL_HTTP_HELPER_H
#define GCAL_HTTP_HELPER_H

#include <span.h>
#include <stdint.h>

typedef struct
{
    /* TLS / connection settings */
    const char* host;             /* "oauth2.googleapis.com" etc. */
    int         port;             /* 443 */
    const char* ca_bundle_path;   /* e.g. /etc/ssl/certs/ca-certificates.crt */

    /* Request line */
    const char* method;           /* "GET" / "POST" */
    const char* path;             /* e.g. "/token" */

    /* Optional bearer token (NULL if none). */
    const char* bearer_token;

    /* Optional form/JSON body. Empty span for GET. */
    span_t      body;
    const char* content_type;     /* required if body is non-empty */
} http_helper_req_t;

typedef struct
{
    int    status;        /* numeric HTTP status (e.g. 200) */
    span_t body;          /* slice into the caller-supplied recv buffer */
} http_helper_resp_t;

/* Performs the HTTPS round-trip. recv_buf is used both for the raw response
 * bytes and as the backing storage for resp->body. Returns 0 on success,
 * non-zero on transport / parse failure. */
int http_helper_do(const http_helper_req_t* req,
                   uint8_t* recv_buf, uint32_t recv_buf_size,
                   http_helper_resp_t* resp);

#endif /* GCAL_HTTP_HELPER_H */
