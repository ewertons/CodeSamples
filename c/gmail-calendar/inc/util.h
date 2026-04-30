/*
 * util.h — small helpers shared across the sample.
 */

#ifndef GCAL_UTIL_H
#define GCAL_UTIL_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* base64url encode `in` into `out` (NUL-terminated). Returns bytes written
 * (excluding the NUL), or -1 on overflow. No padding. */
int b64url_encode(const uint8_t* in, size_t in_len, char* out, size_t out_cap);

/* SHA-256 of `in` into 32-byte `out`. */
void sha256_buf(const uint8_t* in, size_t in_len, uint8_t out[32]);

/* Fill `out` with cryptographically random bytes. Returns true on success. */
bool rand_buf(uint8_t* out, size_t n);

/* Read full file into `buf` (NUL-terminated). Returns bytes read (excluding
 * NUL), or -1 on error / overflow. */
long read_file(const char* path, char* buf, size_t cap);

/* Write `buf` of length `n` to `path` atomically (write+rename). chmod 600
 * on success. Returns true on success. */
bool write_file_secure(const char* path, const char* buf, size_t n);

/* Launch a URL in the user's browser (xdg-open). Non-blocking. */
bool open_browser(const char* url);

/* URL-encode `in` into `out` (NUL-terminated). Returns bytes written or -1
 * on overflow. */
int url_encode(const char* in, char* out, size_t out_cap);

/* Trim trailing newline / CR from a NUL-terminated string in-place. */
void rstrip(char* s);

#endif /* GCAL_UTIL_H */
