# Reading a Google Calendar from a C application

Short answer: **yes, it's possible**, but Google does not ship an official C SDK
for the Calendar API. You'll talk to it over plain HTTPS + JSON, which means in
C you typically combine:

- **libcurl** — HTTPS requests
- **a JSON parser** — e.g. `cJSON`, `json-c`, or `jansson`
- **OAuth 2.0 flow** — to get an access token (Google Calendar API requires
  OAuth; plain API keys don't work for user calendar data)

Note: it's the **Google Calendar API**, not "Gmail Calendar". Even though the
calendar lives in your Gmail account, the API surface is
`calendar.googleapis.com`, not Gmail.

---

## 1. Configuration on the Google side

You do this once, in the Google Cloud Console (https://console.cloud.google.com),
with the Google account whose calendar you want to read.

1. **Create a project** (or pick an existing one).
2. **Enable the Google Calendar API** for that project (APIs & Services →
   Library → "Google Calendar API" → Enable).
3. **Configure the OAuth consent screen**:
   - User type: *External* (unless you're in a Workspace org and want *Internal*).
   - App name, support email, developer email.
   - **Scopes**: add at least `https://www.googleapis.com/auth/calendar.readonly`
     (read-only) or `https://www.googleapis.com/auth/calendar.events.readonly`.
   - **Test users**: while the app is in "Testing" status, add your Gmail
     address as a test user. Otherwise Google will refuse to issue tokens.
4. **Create OAuth 2.0 credentials** (APIs & Services → Credentials → Create
   Credentials → OAuth client ID):
   - For a local C app, choose **"Desktop app"** (a.k.a. "Installed
     application"). This gives you a **client ID** and **client secret**.
   - Avoid the "Web application" type unless you actually have a web server
     with a fixed redirect URI.
5. **Pick a redirect URI strategy**:
   - Easiest: `http://127.0.0.1:<port>` — your C app spins up a tiny local
     HTTP listener to catch the auth code.
   - Alternative: the legacy "out-of-band" / `urn:ietf:wg:oauth:2.0:oob` flow
     is **deprecated** and shouldn't be used for new apps.
6. (Optional) If you don't want to deal with OAuth at all and the calendar
   belongs to a service you control (e.g. a Workspace org), use a **Service
   Account** with **domain-wide delegation**. Not applicable to a personal
   Gmail calendar.

---

## 2. The OAuth 2.0 flow your C app needs to implement

For a desktop app talking to a personal Gmail account, the standard is
**Authorization Code flow with PKCE**:

1. Generate a PKCE `code_verifier` + `code_challenge`.
2. Open the user's browser to:

   ```
   https://accounts.google.com/o/oauth2/v2/auth
     ?client_id=...
     &redirect_uri=http://127.0.0.1:PORT
     &response_type=code
     &scope=https://www.googleapis.com/auth/calendar.readonly
     &code_challenge=...
     &code_challenge_method=S256
     &access_type=offline      (so you also get a refresh_token)
     &prompt=consent
   ```

3. Run a tiny local HTTP server on `127.0.0.1:PORT` and read the `?code=...`
   parameter Google redirects to.
4. POST to `https://oauth2.googleapis.com/token` with
   `grant_type=authorization_code`, the code, the verifier, client ID, client
   secret, redirect URI → get back `access_token` + `refresh_token`.
5. Store the `refresh_token` somewhere safe (e.g. a file with restrictive
   permissions, or the OS keystore). On later runs, exchange it for a new
   `access_token` without bothering the user.

---

## 3. Calling the Calendar API

Once you have an access token, it's just HTTPS GETs with
`Authorization: Bearer <token>`. Two endpoints you'll likely use:

- List calendars:

  ```
  GET https://www.googleapis.com/calendar/v3/users/me/calendarList
  ```

- List events on a calendar (use `primary` for the main one):

  ```
  GET https://www.googleapis.com/calendar/v3/calendars/primary/events
        ?timeMin=2026-04-29T00:00:00Z
        &singleEvents=true
        &orderBy=startTime
        &maxResults=50
  ```

The response is JSON — parse with `cJSON` / `json-c` / `jansson`.

---

## 4. Practical recommendations

- This will be **a lot more code than the equivalent Python/C# sample** (Google
  publishes official client libs for those). If C isn't a hard requirement,
  prototype in Python first, then port the HTTP calls.
- Keep `client_secret` out of source control. For a desktop app it's not really
  "secret" (Google's docs acknowledge this), but still don't commit it.
- Token storage on disk should be `chmod 600` on Linux / ACL-restricted on
  Windows.
- Watch out for clock skew — token expiry is ~1 hour.
