#include "calendar.h"
#include "http_helper.h"

#include <json.h>

#include <stdio.h>
#include <string.h>
#include <time.h>

#define CAL_HOST "www.googleapis.com"

/* ISO 8601 / RFC 3339 in UTC. */
static void iso8601_utc_now(char* buf, size_t cap)
{
    time_t now = time(NULL);
    struct tm tm;
    gmtime_r(&now, &tm);
    strftime(buf, cap, "%Y-%m-%dT%H:%M:%SZ", &tm);
}

typedef struct
{
    char summary[256];
    char start[64];
    char end[64];
    char location[256];
} event_t;

/* Copy a string token into a NUL-terminated char buffer. */
static void copy_string_token(const json_token_t* tok, char* dest, size_t cap)
{
    span_t out;
    if (!failed(json_token_get_string_span(
            tok,
            span_init((uint8_t*)dest, (uint32_t)(cap - 1)),
            &out)))
    {
        dest[span_get_size(out)] = '\0';
    }
}

/* Visitor: parse one event object from the items array. */
static result_t visit_event(json_reader_t* r, uint32_t index, void* ctx)
{
    (void)ctx;
    if (r->token.kind != json_token_begin_object) return ok;

    event_t e = {0};

    /* Walk every property of the event object. */
    while (true)
    {
        result_t rr = json_reader_next_token(r);
        if (failed(rr)) return rr;
        if (r->token.kind == json_token_end_object) break;
        if (r->token.kind != json_token_property_name) return unexpected_char;

        bool is_summary  = json_token_is_text_equal(&r->token,
                            span_from_str_literal("summary"));
        bool is_location = json_token_is_text_equal(&r->token,
                            span_from_str_literal("location"));
        bool is_start    = json_token_is_text_equal(&r->token,
                            span_from_str_literal("start"));
        bool is_end      = json_token_is_text_equal(&r->token,
                            span_from_str_literal("end"));

        rr = json_reader_next_token(r);
        if (failed(rr)) return rr;

        if (is_summary && r->token.kind == json_token_string)
        {
            copy_string_token(&r->token, e.summary, sizeof e.summary);
        }
        else if (is_location && r->token.kind == json_token_string)
        {
            copy_string_token(&r->token, e.location, sizeof e.location);
        }
        else if ((is_start || is_end)
                 && r->token.kind == json_token_begin_object)
        {
            char* dest = is_start ? e.start : e.end;
            size_t cap = is_start ? sizeof e.start : sizeof e.end;

            json_token_t tok;
            if (failed(json_reader_find_property(
                    r, span_from_str_literal("dateTime"), &tok)))
            {
                /* All-day events have "date" instead of "dateTime";
                 * find_property left us at end_object already. */
                snprintf(dest, cap, "(all-day)");
            }
            else
            {
                copy_string_token(&tok, dest, cap);
                /* Skip remaining properties of the start/end sub-object. */
                while (true)
                {
                    rr = json_reader_next_token(r);
                    if (failed(rr)) return rr;
                    if (r->token.kind == json_token_end_object) break;
                    if (r->token.kind == json_token_property_name)
                    {
                        rr = json_reader_next_token(r);
                        if (failed(rr)) return rr;
                        rr = json_reader_skip_children(r);
                        if (failed(rr)) return rr;
                    }
                }
            }
        }
        else
        {
            rr = json_reader_skip_children(r);
            if (failed(rr)) return rr;
        }
    }

    printf("  [%u] %s\n", (unsigned)(index + 1),
           e.summary[0] ? e.summary : "(no title)");
    if (e.start[0])    printf("       start: %s\n", e.start);
    if (e.end[0])      printf("       end:   %s\n", e.end);
    if (e.location[0]) printf("       at:    %s\n", e.location);
    return ok;
}

bool calendar_list_events(const gcal_oauth_t* cfg,
                          const char* access_token,
                          int max_events)
{
    char now[32];
    iso8601_utc_now(now, sizeof now);

    char path[512];
    snprintf(path, sizeof path,
        "/calendar/v3/calendars/primary/events"
        "?timeMin=%s"
        "&singleEvents=true"
        "&orderBy=startTime"
        "&maxResults=%d",
        now, max_events);

    static uint8_t recv_buf[64 * 1024];
    http_helper_req_t req = {
        .host           = CAL_HOST,
        .port           = 443,
        .ca_bundle_path = cfg->ca_bundle_path,
        .method         = "GET",
        .path           = path,
        .bearer_token   = access_token,
        .body           = SPAN_EMPTY,
        .content_type   = NULL,
    };
    http_helper_resp_t resp;
    if (http_helper_do(&req, recv_buf, sizeof recv_buf, &resp) != 0)
    {
        return false;
    }
    if (resp.status / 100 != 2)
    {
        fprintf(stderr, "calendar: HTTP %d\n  %.*s\n", resp.status,
                (int)span_get_size(resp.body),
                (const char*)span_get_ptr(resp.body));
        return false;
    }

    json_reader_t r;
    if (failed(json_reader_init(&r, resp.body, NULL))) return false;
    if (failed(json_reader_next_token(&r))) return false;
    if (r.token.kind != json_token_begin_object)
    {
        fprintf(stderr, "calendar: response is not a JSON object\n");
        return false;
    }

    json_token_t items_tok;
    if (failed(json_reader_find_property(
            &r, span_from_str_literal("items"), &items_tok)))
    {
        fprintf(stderr, "calendar: response missing 'items'\n");
        return false;
    }
    if (r.token.kind != json_token_begin_array)
    {
        fprintf(stderr, "calendar: 'items' is not an array\n");
        return false;
    }

    printf("\nUpcoming events from primary calendar (%s onward):\n", now);
    result_t rr = json_reader_for_each_array_element(&r, visit_event, NULL);
    if (failed(rr))
    {
        fprintf(stderr, "calendar: parse error 0x%x\n", (unsigned)rr);
        return false;
    }
    return true;
}
