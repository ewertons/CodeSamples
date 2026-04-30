/*
 * gcal_c_sample — read events from the user's primary Google Calendar.
 *
 * Modes:
 *   --auth     Run the interactive OAuth flow to obtain a refresh token.
 *              Required once; persists tokens to ./config.json.
 *   (default)  Use the saved refresh token to list upcoming events.
 *
 * See README.md for Google Cloud Console setup.
 */

#include "calendar.h"
#include "oauth.h"

#include <stdio.h>
#include <string.h>

#define CONFIG_PATH "config.json"
#define MAX_EVENTS 10

static int usage(void)
{
    fprintf(stderr,
        "usage: gcal_c_sample [--auth | --list]\n"
        "  --auth    perform OAuth flow and store refresh token\n"
        "  --list    (default) print upcoming events\n");
    return 1;
}

int main(int argc, char** argv)
{
    bool do_auth = false;
    if (argc > 2) return usage();
    if (argc == 2)
    {
        if      (strcmp(argv[1], "--auth") == 0) do_auth = true;
        else if (strcmp(argv[1], "--list") == 0) do_auth = false;
        else return usage();
    }

    gcal_oauth_t cfg;
    if (!oauth_load_config(&cfg, CONFIG_PATH))
    {
        fprintf(stderr,
            "\nFailed to load %s.\n"
            "Create a file with at least:\n"
            "  {\n"
            "    \"client_id\":     \"<from Google Cloud Console>\",\n"
            "    \"client_secret\": \"<from Google Cloud Console>\"\n"
            "  }\n", CONFIG_PATH);
        return 1;
    }

    if (do_auth || !cfg.have_refresh_token)
    {
        if (!oauth_run_auth_flow(&cfg)) return 2;
        if (do_auth) return 0;
    }

    char access_token[GCAL_TOKEN_MAX];
    if (!oauth_get_access_token(&cfg, access_token, sizeof access_token))
    {
        return 3;
    }

    if (!calendar_list_events(&cfg, access_token, MAX_EVENTS))
    {
        return 4;
    }
    return 0;
}
