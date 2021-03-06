/*
 * daemonlib
 * Copyright (C) 2012-2014, 2016, 2019-2021 Matthias Bolte <matthias@tinkerforge.com>
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * log.h: Logging specific functions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef DAEMONLIB_LOG_H
#define DAEMONLIB_LOG_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#ifndef _MSC_VER
	#include <sys/time.h>
#endif

#include "io.h"
#include "macros.h"

typedef enum {
	LOG_LEVEL_NONE = -1, // special value, force signed enum to avoid "unsigned expression >= 0 is always true" warnings
	LOG_LEVEL_ERROR = 0,
	LOG_LEVEL_WARN,
	LOG_LEVEL_INFO,
	LOG_LEVEL_DEBUG
} LogLevel;

typedef int (*LogRotateFunction)(IO *output, LogLevel *level, char *message, int message_length);

typedef enum { // bitmask
	LOG_DEBUG_GROUP_NONE   = 0x0000, // special value
	LOG_DEBUG_GROUP_COMMON = 0x0001,
	LOG_DEBUG_GROUP_EVENT  = 0x0002,
	LOG_DEBUG_GROUP_PACKET = 0x0004,
	LOG_DEBUG_GROUP_OBJECT = 0x0008,
	LOG_DEBUG_GROUP_LIBUSB = 0x0010
} LogDebugGroup;

#define LOG_DEBUG_GROUP_ALL (LOG_DEBUG_GROUP_COMMON | \
                             LOG_DEBUG_GROUP_EVENT | \
                             LOG_DEBUG_GROUP_PACKET | \
                             LOG_DEBUG_GROUP_OBJECT | \
                             LOG_DEBUG_GROUP_LIBUSB)

typedef enum { // bitmask
	LOG_INCLUSION_NONE      = 0x0000, // special value
	LOG_INCLUSION_PRIMARY   = 0x0001,
	LOG_INCLUSION_SECONDARY = 0x0002
} LogInclusion;

#define LOG_MAX_SOURCE_LINES 16

typedef struct {
	int number;
	uint32_t included_debug_groups;
} LogSourceLine;

typedef struct {
	const char *file; // __FILE__
	const char *name; // last part of __FILE__
	int debug_filter_version;
	uint32_t included_debug_groups;
	LogSourceLine lines[LOG_MAX_SOURCE_LINES];
	int lines_used;
	bool libusb;
} LogSource;

#define LOG_SOURCE_INITIALIZER { \
		__FILE__, \
		NULL, \
		-1, \
		LOG_DEBUG_GROUP_ALL, \
		{{0}}, \
		0, \
		false \
	}

#ifdef DAEMONLIB_WITH_LOGGING
	#ifdef _MSC_VER
		#define log_message_checked(level, debug_group, ...) \
			do { \
				uint32_t _inclusion_ = log_check_inclusion(level, &_log_source, debug_group, __LINE__); \
				if (_inclusion_ != LOG_INCLUSION_NONE) { \
					log_message(level, &_log_source, debug_group, _inclusion_, __func__, __LINE__, __VA_ARGS__); \
				} \
			__pragma(warning(push)) \
			__pragma(warning(disable:4127)) \
			} while (0) \
			__pragma(warning(pop))
	#else
		#define log_message_checked(level, debug_group, ...) \
			do { \
				uint32_t _inclusion_ = log_check_inclusion(level, &_log_source, debug_group, __LINE__); \
				if (_inclusion_ != LOG_INCLUSION_NONE) { \
					log_message(level, &_log_source, debug_group, _inclusion_, __func__, __LINE__, __VA_ARGS__); \
				} \
			} while (0)
	#endif

	#define log_error(...) log_message_checked(LOG_LEVEL_ERROR, LOG_DEBUG_GROUP_NONE, __VA_ARGS__)
	#define log_warn(...)  log_message_checked(LOG_LEVEL_WARN, LOG_DEBUG_GROUP_NONE, __VA_ARGS__)
	#define log_info(...)  log_message_checked(LOG_LEVEL_INFO, LOG_DEBUG_GROUP_NONE, __VA_ARGS__)
	#define log_debug(...) log_message_checked(LOG_LEVEL_DEBUG, LOG_DEBUG_GROUP_COMMON, __VA_ARGS__)

	// special debug logging for high traffic debug messages from event, packet
	// and object related functions. the visibility of these messages can be
	// controlled with the event, packet and object debug filters
	#define log_event_debug(...)  log_message_checked(LOG_LEVEL_DEBUG, LOG_DEBUG_GROUP_EVENT, __VA_ARGS__)
	#define log_packet_debug(...) log_message_checked(LOG_LEVEL_DEBUG, LOG_DEBUG_GROUP_PACKET, __VA_ARGS__)
	#define log_object_debug(...) log_message_checked(LOG_LEVEL_DEBUG, LOG_DEBUG_GROUP_OBJECT, __VA_ARGS__)
#else
	#define log_error(...)        ((void)0)
	#define log_warn(...)         ((void)0)
	#define log_info(...)         ((void)0)
	#define log_debug(...)        ((void)0)
	#define log_event_debug(...)  ((void)0)
	#define log_packet_debug(...) ((void)0)
	#define log_object_debug(...) ((void)0)
#endif

extern IO log_stderr_output;

void log_init(void);
void log_exit(void);

void log_enable_debug_override(const char *filter);

LogLevel log_get_effective_level(void);

void log_set_output(IO *output, LogRotateFunction rotate);
void log_get_output(IO **output, LogRotateFunction *rotate);

uint32_t log_check_inclusion(LogLevel level, LogSource *source,
                             LogDebugGroup debug_group, int line);

void log_message(LogLevel level, LogSource *source, LogDebugGroup debug_group,
                 uint32_t inclusion, const char *function, int line,
                 const char *format, ...) ATTRIBUTE_FMT_PRINTF(7, 8);

int log_format(char *buffer, int length, struct timeval *timestamp,
               LogLevel level, LogSource *source, LogDebugGroup debug_group,
               const char *function, int line, const char *message);

#endif // DAEMONLIB_LOG_H
