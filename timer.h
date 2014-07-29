/*
 * daemonlib
 * Copyright (C) 2014 Matthias Bolte <matthias@tinkerforge.com>
 *
 * timer.h: Timer specific functions
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

#ifndef DAEMONLIB_TIMER_H
#define DAEMONLIB_TIMER_H

#include <stdint.h>

#ifdef _WIN32
	#include "timer_winapi.h"
#elif defined (__linux__)
	#include "timer_linux.h"
#elif defined (__APPLE__)
	#include "timer_macosx.h"
#else
	#error unknown platform
#endif

int timer_create_(Timer *timer, TimerFunction function, void *opaque);
void timer_destroy(Timer *timer);

int timer_configure(Timer *timer, uint64_t delay, uint64_t interval); // microseconds

#endif // DAEMONLIB_TIMER_H