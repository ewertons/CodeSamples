/*
 * calendar.h
 *
 * Reads upcoming events from the user's primary Google Calendar and prints
 * them to stdout. Uses an access token obtained via oauth.h.
 */

#ifndef GCAL_CALENDAR_H
#define GCAL_CALENDAR_H

#include "oauth.h"
#include <stdbool.h>

/* Fetch up to `max_events` events starting at "now" from the primary
 * calendar and print them to stdout. */
bool calendar_list_events(const gcal_oauth_t* cfg,
                          const char* access_token,
                          int max_events);

#endif /* GCAL_CALENDAR_H */
