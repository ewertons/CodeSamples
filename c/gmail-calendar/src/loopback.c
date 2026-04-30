/*
 * loopback.c
 *
 * Single-shot OAuth-redirect listener built on http-c's plain-HTTP server.
 *
 * Flow:
 *   1. Initialise an `http_server_t` with tls.enable=false on the
 *      caller-supplied port.
 *   2. Register a single GET route matching "^/.*$" — Google redirects
 *      to "/?code=...&state=...".
 *   3. Run the server asynchronously on a worker task.
 *   4. When the route handler fires, copy out the `code` query
 *      parameter, signal a condition variable, and reply 200 OK with a
 *      "you may close this tab" body.
 *   5. Main thread wakes from the condvar (or times out), stops the
 *      server, and returns.
 */

#include "loopback.h"

#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "common_lib_c.h"

#include <http_request.h>
#include <http_response.h>
#include "http_codes.h"
#include "http_headers.h"
#include "http_methods.h"
#include "http_query.h"
#include "http_server.h"
#include "http_server_storage.h"
#include "http_versions.h"
#include "task.h"

#define LOOPBACK_CODE_MAX 1024

typedef struct loopback_ctx
{
    pthread_mutex_t mutex;
    pthread_cond_t  cond;
    bool            received;
    bool            success;
    char            code[LOOPBACK_CODE_MAX];
    size_t          code_len;
} loopback_ctx_t;

/* Body served back to the browser tab. Static so we can take its address
 * as the response body without copying. */
static const char RESPONSE_HTML[] =
    "<!doctype html><html><body style='font-family:sans-serif'>"
    "<h2>Authorization received</h2>"
    "<p>You can close this tab and return to the terminal.</p>"
    "</body></html>";
#define RESPONSE_HTML_LEN ((uint32_t)(sizeof RESPONSE_HTML - 1))

static const span_t CONTENT_TYPE_HTML =
    span_from_str_literal("text/html; charset=utf-8");

/* Per-handler scratch buffers. The response is fully serialised into
 * the connection's send buffer before the handler returns, so reusing
 * statics across invocations is safe (and our flow only handles one
 * request anyway). */
static uint8_t s_response_headers_buf[256];
static char    s_content_length_str[16];

static void oauth_callback_handler(http_request_t* request,
                                   span_t* matches, uint16_t match_count,
                                   http_response_t* out, void* ctx)
{
    (void)matches;
    (void)match_count;

    loopback_ctx_t* lc = (loopback_ctx_t*)ctx;

    span_t code_span  = SPAN_EMPTY;
    span_t error_span = SPAN_EMPTY;
    (void)http_query_find(request->path,
                          span_from_str_literal("code"),  &code_span);
    (void)http_query_find(request->path,
                          span_from_str_literal("error"), &error_span);

    (void)pthread_mutex_lock(&lc->mutex);
    if (!lc->received)
    {
        uint32_t cz = span_get_size(code_span);
        if (cz > 0 && cz < sizeof lc->code)
        {
            memcpy(lc->code, span_get_ptr(code_span), cz);
            lc->code_len      = cz;
            lc->code[cz]      = '\0';
            lc->success       = true;
        }
        else if (span_get_size(error_span) > 0)
        {
            fprintf(stderr,
                    "loopback: OAuth error from Google: %.*s\n",
                    (int)span_get_size(error_span),
                    (const char*)span_get_ptr(error_span));
        }
        lc->received = true;
        (void)pthread_cond_broadcast(&lc->cond);
    }
    (void)pthread_mutex_unlock(&lc->mutex);

    /* Build the 200 OK response. The framework pre-fills `out` with
     * defaults; we attach a header buffer plus Content-Length /
     * Content-Type and point body at the static HTML literal. */
    if (http_headers_init(&out->headers,
                          span_init(s_response_headers_buf,
                                    (uint32_t)sizeof s_response_headers_buf))
        != HL_RESULT_OK)
    {
        return;
    }

    int n = snprintf(s_content_length_str, sizeof s_content_length_str,
                     "%u", (unsigned)RESPONSE_HTML_LEN);
    if (n <= 0 || (size_t)n >= sizeof s_content_length_str) { return; }
    span_t clen_span = span_init((uint8_t*)s_content_length_str, (uint32_t)n);

    (void)http_headers_add(&out->headers,
                           HTTP_HEADER_CONTENT_TYPE,   CONTENT_TYPE_HTML);
    (void)http_headers_add(&out->headers,
                           HTTP_HEADER_CONTENT_LENGTH, clen_span);

    out->code          = HTTP_CODE_200;
    out->reason_phrase = HTTP_REASON_PHRASE_200;
    out->body          = span_init((uint8_t*)RESPONSE_HTML, RESPONSE_HTML_LEN);
}

bool loopback_wait_for_code(int port, int timeout_secs,
                            char* code_buf, size_t code_cap)
{
    if (code_buf == NULL || code_cap == 0) { return false; }

    loopback_ctx_t lc;
    memset(&lc, 0, sizeof lc);
    if (pthread_mutex_init(&lc.mutex, NULL) != 0) { return false; }
    if (pthread_cond_init(&lc.cond, NULL) != 0)
    {
        (void)pthread_mutex_destroy(&lc.mutex);
        return false;
    }

    if (task_platform_init() != ok)
    {
        fprintf(stderr, "loopback: task_platform_init failed\n");
        (void)pthread_cond_destroy(&lc.cond);
        (void)pthread_mutex_destroy(&lc.mutex);
        return false;
    }

    http_server_t        server;
    http_server_config_t cfg = http_server_get_default_config();
    cfg.port                  = port;
    cfg.tls.enable            = false;
    cfg.tls.certificate_file  = NULL;
    cfg.tls.private_key_file  = NULL;

    if (http_server_init(&server, &cfg,
                         http_server_storage_get_for_server_host()) != ok)
    {
        fprintf(stderr,
                "loopback: http_server_init failed (port %d already in use?)\n",
                port);
        (void)task_platform_deinit();
        (void)pthread_cond_destroy(&lc.cond);
        (void)pthread_mutex_destroy(&lc.mutex);
        return false;
    }

    if (http_server_add_route(&server, HTTP_METHOD_GET,
                              span_from_str_literal("^/.*$"),
                              oauth_callback_handler, &lc) != ok)
    {
        fprintf(stderr, "loopback: http_server_add_route failed\n");
        (void)http_server_deinit(&server);
        (void)task_platform_deinit();
        (void)pthread_cond_destroy(&lc.cond);
        (void)pthread_mutex_destroy(&lc.mutex);
        return false;
    }

    task_t* run_task = http_server_run_async(&server);
    if (run_task == NULL)
    {
        fprintf(stderr, "loopback: http_server_run_async failed\n");
        (void)http_server_deinit(&server);
        (void)task_platform_deinit();
        (void)pthread_cond_destroy(&lc.cond);
        (void)pthread_mutex_destroy(&lc.mutex);
        return false;
    }

    /* Wait (with timeout) for the handler to capture the auth code. */
    struct timespec deadline;
    (void)clock_gettime(CLOCK_REALTIME, &deadline);
    deadline.tv_sec += timeout_secs;

    bool timed_out = false;
    (void)pthread_mutex_lock(&lc.mutex);
    while (!lc.received && !timed_out)
    {
        int rc = pthread_cond_timedwait(&lc.cond, &lc.mutex, &deadline);
        if (rc == ETIMEDOUT) { timed_out = true; }
    }

    bool got_code = false;
    if (lc.received && lc.success && lc.code_len > 0 && lc.code_len < code_cap)
    {
        memcpy(code_buf, lc.code, lc.code_len);
        code_buf[lc.code_len] = '\0';
        got_code = true;
    }
    (void)pthread_mutex_unlock(&lc.mutex);

    if (timed_out)
    {
        fprintf(stderr, "loopback: timed out waiting for redirect.\n");
    }

    /* Give the worker a moment to finish flushing the 200 OK response
     * onto the wire before we tear the server down. */
    (void)usleep(200 * 1000);

    (void)http_server_stop(&server);
    (void)task_wait(run_task);
    task_release(run_task);
    (void)http_server_deinit(&server);
    (void)task_platform_deinit();

    (void)pthread_cond_destroy(&lc.cond);
    (void)pthread_mutex_destroy(&lc.mutex);
    return got_code;
}
