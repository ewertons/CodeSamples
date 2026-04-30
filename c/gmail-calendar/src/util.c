#include "util.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

/* ------------------------------------------------------------------------- */
/* base64url (no padding) — RFC 4648 §5                                      */
/* ------------------------------------------------------------------------- */

int b64url_encode(const uint8_t* in, size_t in_len, char* out, size_t out_cap)
{
    /* Encoded length without padding = ceil(in_len * 4 / 3). */
    size_t enc_len = 4 * ((in_len + 2) / 3);
    if (enc_len + 1 > out_cap)
    {
        return -1;
    }

    int n = EVP_EncodeBlock((unsigned char*)out, in, (int)in_len);
    if (n < 0) return -1;

    /* Translate to URL alphabet and strip '=' padding. */
    int w = 0;
    for (int i = 0; i < n; i++)
    {
        char c = out[i];
        if (c == '=') continue;
        if (c == '+') c = '-';
        else if (c == '/') c = '_';
        out[w++] = c;
    }
    out[w] = '\0';
    return w;
}

/* ------------------------------------------------------------------------- */
/* SHA-256                                                                    */
/* ------------------------------------------------------------------------- */

void sha256_buf(const uint8_t* in, size_t in_len, uint8_t out[32])
{
    SHA256(in, in_len, out);
}

/* ------------------------------------------------------------------------- */
/* RNG                                                                        */
/* ------------------------------------------------------------------------- */

bool rand_buf(uint8_t* out, size_t n)
{
    return RAND_bytes(out, (int)n) == 1;
}

/* ------------------------------------------------------------------------- */
/* File I/O                                                                   */
/* ------------------------------------------------------------------------- */

long read_file(const char* path, char* buf, size_t cap)
{
    FILE* f = fopen(path, "rb");
    if (!f) return -1;
    size_t n = fread(buf, 1, cap - 1, f);
    int err = ferror(f);
    fclose(f);
    if (err) return -1;
    buf[n] = '\0';
    return (long)n;
}

bool write_file_secure(const char* path, const char* buf, size_t n)
{
    char tmp[512];
    if (snprintf(tmp, sizeof tmp, "%s.tmp", path) >= (int)sizeof tmp)
    {
        return false;
    }
    int fd = open(tmp, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd < 0) return false;
    bool ok = true;
    size_t off = 0;
    while (off < n)
    {
        ssize_t w = write(fd, buf + off, n - off);
        if (w <= 0) { ok = false; break; }
        off += (size_t)w;
    }
    if (close(fd) != 0) ok = false;
    if (ok && rename(tmp, path) != 0) ok = false;
    if (!ok) (void)unlink(tmp);
    return ok;
}

/* ------------------------------------------------------------------------- */
/* Browser launch                                                             */
/* ------------------------------------------------------------------------- */

bool open_browser(const char* url)
{
    /* Minimal: fork+exec xdg-open, ignore status. */
    pid_t pid = fork();
    if (pid < 0) return false;
    if (pid == 0)
    {
        /* Detach stdio so the child doesn't pollute our terminal. */
        int devnull = open("/dev/null", O_RDWR);
        if (devnull >= 0)
        {
            dup2(devnull, 0);
            dup2(devnull, 1);
            dup2(devnull, 2);
            if (devnull > 2) close(devnull);
        }
        execlp("xdg-open", "xdg-open", url, (char*)NULL);
        _exit(127);
    }
    return true;
}

/* ------------------------------------------------------------------------- */
/* URL encoding                                                               */
/* ------------------------------------------------------------------------- */

static int is_unreserved(char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
        || (c >= '0' && c <= '9')
        || c == '-' || c == '_' || c == '.' || c == '~';
}

int url_encode(const char* in, char* out, size_t out_cap)
{
    size_t w = 0;
    for (const char* p = in; *p; p++)
    {
        if (is_unreserved(*p))
        {
            if (w + 1 >= out_cap) return -1;
            out[w++] = *p;
        }
        else
        {
            if (w + 3 >= out_cap) return -1;
            static const char hex[] = "0123456789ABCDEF";
            out[w++] = '%';
            out[w++] = hex[(unsigned char)*p >> 4];
            out[w++] = hex[(unsigned char)*p & 0xF];
        }
    }
    if (w >= out_cap) return -1;
    out[w] = '\0';
    return (int)w;
}

void rstrip(char* s)
{
    size_t n = strlen(s);
    while (n > 0 && (s[n-1] == '\n' || s[n-1] == '\r' || s[n-1] == ' ' || s[n-1] == '\t'))
    {
        s[--n] = '\0';
    }
}
