/*
 * daemonlib
 * Copyright (C) 2012, 2014, 2021 Matthias Bolte <matthias@tinkerforge.com>
 *
 * threads_winapi.h: WinAPI based thread and locking implementation
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

#ifndef DAEMONLIB_THREADS_WINAPI_H
#define DAEMONLIB_THREADS_WINAPI_H

#include <windows.h>

typedef void (*ThreadFunction)(void *opaque);

typedef struct {
	CRITICAL_SECTION handle;
} Mutex;

typedef struct {
	CONDITION_VARIABLE handle;
} Condition;

typedef struct {
	HANDLE handle;
} Semaphore;

typedef struct {
	HANDLE handle;
	DWORD id;
	ThreadFunction function;
	void *opaque;
} Thread;

#endif // DAEMONLIB_THREADS_WINAPI_H
