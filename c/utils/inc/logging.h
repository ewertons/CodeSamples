// Copyright (c) 2018 Scaboro da Silva, Ewerton. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef LOGGING_H
#define LOGGING_H

#include <stdlib.h>
#include <stddef.h>

typedef enum LOGGING_LEVEL_ENUM
{
    LOGGING_LEVEL_TRACE,
    LOGGING_LEVEL_INFO,
    LOGGING_LEVEL_WARNING,
    LOGGING_LEVEL_ERROR
} LOGGING_LEVEL;

void log_entry(LOGGING_LEVEL level, const char* format, ...);

#if defined _MSC_VER
#define log_info(FORMAT, ...) log_entry(LOGGING_LEVEL_INFO, FORMAT, __VA_ARGS__);
#define log_warning(FORMAT, ...) log_entry(LOGGING_LEVEL_WARNING, FORMAT, __VA_ARGS__);
#define log_error(FORMAT, ...) log_entry(LOGGING_LEVEL_ERROR, FORMAT, __VA_ARGS__);
#define log_trace(FORMAT, ...) log_entry(LOGGING_LEVEL_TRACE, FORMAT, __VA_ARGS__);
#else
#define log_info(FORMAT, ...) log_entry(LOGGING_LEVEL_INFO, FORMAT, ##__VA_ARGS__);
#define log_warning(FORMAT, ...) log_entry(LOGGING_LEVEL_WARNING, FORMAT, ##__VA_ARGS__);
#define log_error(FORMAT, ...) log_entry(LOGGING_LEVEL_ERROR, FORMAT, ##__VA_ARGS__);
#define log_trace(FORMAT, ...) log_entry(LOGGING_LEVEL_TRACE, FORMAT, ##__VA_ARGS__);
#endif

#endif // LOGGING_H