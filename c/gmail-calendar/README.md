# gcal-c-sample — Read your Google Calendar from C

A self-contained C99 proof-of-concept that signs in to a Google account,
asks for read-only access to the user's primary Google Calendar, and
prints the next 10 upcoming events to the console.

## Architecture

| Concern | Library / approach |
|---|---|
| HTTPS client (Google APIs) | [`ewertons/http-c`](https://github.com/ewertons/http-c) |
| JSON parsing | [`ewertons/common-lib-c`](https://github.com/ewertons/common-lib-c) `json_reader_t` (zero-malloc, streaming) used directly via the `json_reader_find_property` / `json_reader_for_each_array_element` helpers |
| OAuth 2.0 PKCE flow | Hand-rolled, OpenSSL for SHA-256 / RNG / base64url |
| OAuth callback receiver | http-c `http_server_t` with `tls.enable=false` ([src/loopback.c](src/loopback.c)) |
| Memory model | No heap allocation in the hot path. All buffers are fixed-size and stack/static |

### Request flow

```
                       (1) /auth  + PKCE  challenge
   user's browser  ─────────────────────────────►  accounts.google.com
        │                                                   │
        │  (2) sign in & consent                             │
        │                                                   │
        │ ◄─────────  302 to  http://127.0.0.1:8765/?code=  ─┘
        ▼
   gcal_c_sample  (loopback listener)
        │
        │  (3) POST /token  (code + verifier)   ─────►  oauth2.googleapis.com
        │ ◄──────────  access_token + refresh_token  ─────────┘
        │
        │  (4) GET /calendar/v3/calendars/primary/events
        │      Authorization: Bearer <access_token>          ──►  www.googleapis.com
        │ ◄──────────  JSON list of events  ─────────────────────┘
        ▼
   json_reader_t →  printf
```

---

## Part 1 — Configure your Google account

You only have to do this once.

### 1. Create or select a Google Cloud project

Go to <https://console.cloud.google.com/projectcreate>, create a project
(any name), and select it.

Reference: [Cloud Console — Manage projects](https://cloud.google.com/resource-manager/docs/creating-managing-projects).

### 2. Enable the Google Calendar API

Open <https://console.cloud.google.com/apis/library/calendar-json.googleapis.com>
and click **Enable**.

Reference: [Calendar API — getting started](https://developers.google.com/calendar/api/quickstart/go#enable_the_api).

### 3. Configure the OAuth consent screen

Go to **APIs & Services → OAuth consent screen** (<https://console.cloud.google.com/apis/credentials/consent>).

| Field | Value |
|---|---|
| User type | **External** |
| App name | `gcal-c-sample` (any) |
| User support email / Developer email | your email |
| Scopes | add `https://www.googleapis.com/auth/calendar.readonly` |
| Test users | **add your own Gmail address** |

While the app is in **Testing** status (the default), Google will issue
tokens only for users listed under *Test users*. That's fine for a
proof-of-concept — you never have to publish the app.

Reference: [Configure the OAuth consent screen](https://developers.google.com/workspace/guides/configure-oauth-consent).

### 4. Create OAuth 2.0 credentials

Go to **APIs & Services → Credentials** → **Create Credentials → OAuth client ID**
(<https://console.cloud.google.com/apis/credentials>).

| Field | Value |
|---|---|
| Application type | **Desktop app** |
| Name | `gcal-c-sample` (any) |

Click **Create**. Google will show you a dialog with **Client ID** and
**Client secret** — copy both.

> **Why "Desktop app"?** It is the only type that supports the
> `http://127.0.0.1:PORT` loopback redirect without a pre-registered
> redirect URI list. The "client_secret" of a desktop app is not really
> secret, but Google still issues one — keep it out of source control.

References:
- [OAuth 2.0 for Desktop / Installed Applications](https://developers.google.com/identity/protocols/oauth2/native-app)
- [Loopback IP address (the redirect mechanism this sample uses)](https://developers.google.com/identity/protocols/oauth2/native-app#redirect-uri_loopback)

### 5. Drop the credentials into `config.json`

Create `c/gmail-calendar/config.json` with the values you just got:

```json
{
  "client_id":     "1234567890-abcdefg.apps.googleusercontent.com",
  "client_secret": "GOCSPX-...........",
  "loopback_port": 8765
}
```

`config.json` is git-ignored. **Do not commit it.**

Optional fields:

| Key | Default | Notes |
|---|---|---|
| `loopback_port` | `8765` | The port the OAuth callback listener binds to |
| `ca_bundle_path` | `/etc/ssl/certs/ca-certificates.crt` | Used by http-c to verify Google's TLS certs |

---

## Part 2 — Build (Ubuntu 24.04)

### 2a. Install build prerequisites

```bash
sudo apt update
sudo apt install -y \
    build-essential cmake pkg-config git \
    libssl-dev \
    ca-certificates \
    xdg-utils
# cmocka is needed by http-c's tests, harmless if absent for our build:
sudo apt install -y libcmocka-dev || true
```

### 2b. Build

```bash
cd c/gmail-calendar
./build.sh
```

The first configure is slow — CMake fetches `http-c` and
`common-lib-c` (transitive via http-c) via `FetchContent` and builds them. Subsequent builds
are incremental.

The binary lands at `build/gcal_c_sample`.

---

## Part 3 — Run

### 3a. First run — obtain a refresh token

```bash
cd c/gmail-calendar
./build/gcal_c_sample --auth
```

What you'll see:

```
Opening your browser for Google sign-in...
If it does not open automatically, paste this URL manually:

  https://accounts.google.com/o/oauth2/v2/auth?client_id=...

Waiting for redirect on http://127.0.0.1:8765 ...
```

Your default browser should open with Google's sign-in screen. After
sign-in you will see:

1. **"Choose an account"** — pick the one you registered as a test user.
2. **"Google hasn't verified this app"** — click *Advanced* → *Go to
   gcal-c-sample (unsafe)*. This warning is normal for an
   in-development OAuth app and is a cosmetic only.
3. **"gcal-c-sample wants to access your Google Account"** — click
   *Continue*. Confirm the permission `See and download any calendar
   you can access using your Google Calendar`.

Google then redirects to `http://127.0.0.1:8765/?code=...`. Our loopback
listener captures the code and the browser tab shows:

> **Authorization received**
> You can close this tab and return to the terminal.

Back in the terminal:

```
Got authorization code (length=73).
Refresh token saved to config.json
```

`config.json` now contains a `refresh_token` field. **Treat it like a
password** — anyone with it can read your calendar until you revoke
access at <https://myaccount.google.com/permissions>.

#### Running on a headless machine (no local browser)

`127.0.0.1:8765` is a *loopback* address — it never leaves the box, so
no router / firewall changes are required. But on a headless server
there is no browser to receive the redirect. The simplest fix is an
**SSH local-port-forward** from a workstation that *does* have a
browser:

```bash
# From your workstation (the machine with the browser):
ssh -L 8765:127.0.0.1:8765 user@headless-host

# Then, in that SSH session, on the headless host:
./build/gcal_c_sample --auth
```

Copy the `https://accounts.google.com/o/oauth2/v2/auth?...` URL the
sample prints and paste it into the browser **on your workstation**.
After consent, Google redirects the workstation browser to
`http://127.0.0.1:8765/?code=...`. The SSH tunnel transparently
forwards that connection to the listener on the headless host, which
captures the code and writes `config.json` exactly as in the local case.

No public DNS, no opened ports, no router config. Once `config.json`
has a refresh token, the headless host can run `gcal_c_sample` on its
own indefinitely — the tunnel is only needed for this one-time consent.

### 3b. Subsequent runs — list events

```bash
./build/gcal_c_sample
```

Sample output:

```
Upcoming events from primary calendar (2026-04-29T17:34:00Z onward):
  [1] Team standup
       start: 2026-04-30T09:00:00-07:00
       end:   2026-04-30T09:30:00-07:00
  [2] 1:1 with Alex
       start: 2026-04-30T14:00:00-07:00
       end:   2026-04-30T14:30:00-07:00
       at:    Building 99 / Roosevelt
  [3] Dentist
       start: 2026-05-02T11:00:00-07:00
       end:   2026-05-02T12:00:00-07:00
```

The access token has a ~1h lifetime. Each run uses the saved
`refresh_token` to request a fresh access token, then makes the
Calendar API call.

---

## Code map

| File | Purpose |
|---|---|
| [src/main.c](src/main.c) | CLI entry point, mode selection |
| [src/oauth.c](src/oauth.c) / [inc/oauth.h](inc/oauth.h) | Config persistence, PKCE flow, refresh-token grant |
| [src/loopback.c](src/loopback.c) / [inc/loopback.h](inc/loopback.h) | Single-shot plain-HTTP loopback listener (http-c `http_server_t`) for the OAuth redirect |
| [src/http_helper.c](src/http_helper.c) / [inc/http_helper.h](inc/http_helper.h) | Thin wrapper over http-c's HTTPS client |
| [src/calendar.c](src/calendar.c) / [inc/calendar.h](inc/calendar.h) | Calendar API call + per-event printing |
| [src/util.c](src/util.c) / [inc/util.h](inc/util.h) | base64url, SHA-256, RNG, URL-encode, secure file write |
| [docs/google-calendar.md](docs/google-calendar.md) | Background notes on the Google Calendar API in C |

### JSON parsing

The sample reads JSON directly with common-lib-c's `json_reader_t`,
relying on three convenience helpers in `<json.h>`:

```c
result_t json_reader_rewind             (json_reader_t*);
result_t json_reader_find_property      (json_reader_t*, span_t name, json_token_t* out);
result_t json_reader_for_each_array_element(json_reader_t*, json_array_visitor_fn, void* ctx);
result_t json_token_get_string_span     (const json_token_t*, span_t dest, span_t* out_written);
```

A tiny `json_root_find` / `json_root_get_string` pair sits at the top of
[src/oauth.c](src/oauth.c) to dedupe the rewind+find_property idiom for
flat root-level lookups.

---

## Troubleshooting

| Symptom | Likely cause / fix |
|---|---|
| `bind(127.0.0.1:8765) failed: Address already in use` | Another process is bound to the loopback port. Pick another port via `loopback_port` in `config.json` and re-register it as the redirect URI is **not** required for the loopback flow (any port works). |
| `oauth: token endpoint HTTP 400 — invalid_grant` | The auth code expired (codes are valid only ~10 min) or the redirect_uri sent in the token exchange did not match the one used in the auth URL. Re-run `--auth`. |
| `oauth: token endpoint HTTP 401 — invalid_client` | `client_id` or `client_secret` in `config.json` is wrong. Re-copy them from the Cloud Console. |
| `Google did not return a refresh_token` | You previously consented for this client. Revoke at <https://myaccount.google.com/permissions> and re-run `--auth`. |
| `calendar: HTTP 403 — accessNotConfigured` | The Calendar API is not enabled for the project ([fix](https://console.cloud.google.com/apis/library/calendar-json.googleapis.com)). |
| `calendar: HTTP 403 — access_denied` | You signed in with a Google account that is not in the *Test users* list. Add it under the OAuth consent screen settings. |
| TLS `unable to get local issuer certificate` | `ca_bundle_path` in `config.json` is wrong. On Ubuntu 24.04 the correct path is `/etc/ssl/certs/ca-certificates.crt` (provided by the `ca-certificates` package). |

---

## Security notes

- **`config.json` contains long-lived secrets.** It is `chmod 600`'d on
  write, but treat the file as you would an SSH key.
- The sample stores the refresh token on disk in plaintext. A
  production app would use the OS keychain (libsecret, Keychain,
  Credential Manager).
- The OAuth `client_secret` of a Desktop application is not a real
  secret per Google's own docs — but it is still embedded in your
  binary if you hardcode it. Ship it via config, not source.
- Revoke access any time at <https://myaccount.google.com/permissions>.

---

## License

MIT, like the rest of this repo. Bundled dependencies retain their own
licenses (http-c is MIT, common-lib-c is MIT, OpenSSL is Apache-2.0
on Ubuntu 24.04).
