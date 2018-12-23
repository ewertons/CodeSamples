// Copyright (c) 2018 Scaboro da Silva, Ewerton. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "logging.h"
#include <stdarg.h>
#include <time.h>
#include <stdio.h>

#define MAX_TIMESTAMP_LENGTH 26

void log_entry(LOGGING_LEVEL level, const char* format, ...)
{
    time_t now;
    char timestamp[MAX_TIMESTAMP_LENGTH];
    va_list args;
    va_start(args, format);

    now = time(NULL);

    struct tm* timeinfo = localtime((const time_t*)&now);
    size_t timestamp_length = strftime(timestamp, MAX_TIMESTAMP_LENGTH, "%F %H:%M:%S", timeinfo);
    (void)printf("%.24s ",timestamp, timeinfo->tm_sec);

    switch (level)
    {
        case LOGGING_LEVEL_INFO:
            (void)printf("[I]: ");
            break;
        case LOGGING_LEVEL_ERROR:
            (void)printf("[E]: ");
            break;
        case LOGGING_LEVEL_WARNING:
            (void)printf("[W]: ");
            break;
        case LOGGING_LEVEL_TRACE:
            (void)printf("[T]: ");
            break;
        default:
            break;
    }

    va_end(args);

    (void)vprintf(format, args);
    (void)printf("\r\n");
}