/*
 * loopback.h
 *
 * Single-shot HTTP listener on 127.0.0.1, used to receive the OAuth
 * authorization-code redirect from Google's installed-app flow
 * (http://127.0.0.1:PORT, plain HTTP, mandated by Google).
 *
 * Implemented on top of http-c's `http_server_t` with `tls.enable=false`.
 */

#ifndef GCAL_LOOPBACK_H
#define GCAL_LOOPBACK_H

#include <stdbool.h>
#include <stddef.h>

/* Accept one HTTP GET on http://127.0.0.1:port and capture the value of
 * the `code` query parameter into `code_buf` (NUL-terminated).
 *
 * Returns true on success. Times out after `timeout_secs`. */
bool loopback_wait_for_code(int port,
                            int timeout_secs,
                            char* code_buf,
                            size_t code_cap);

#endif /* GCAL_LOOPBACK_H */
