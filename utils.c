/*
 * daemonlib
 * Copyright (C) 2012-2014 Matthias Bolte <matthias@tinkerforge.com>
 * Copyright (C) 2014 Olaf Lüke <olaf@tinkerforge.com>
 *
 * utils.c: Utility functions
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

#include <errno.h>
#ifndef _WIN32
	#include <netdb.h>
	#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MSC_VER
	#include <sys/time.h>
#endif
#ifdef _WIN32
	#include <winsock2.h> // must be included before windows.h
	#include <windows.h>
#endif

#include "utils.h"

#if !defined(_WIN32) && !defined(EAI_ADDRFAMILY)
	#if EAI_AGAIN < 0
		#define EAI_ADDRFAMILY -9
	#else
		#define EAI_ADDRFAMILY 9
	#endif
#endif

int errno_interrupted(void) {
#ifdef _WIN32
	return errno == ERRNO_WINAPI_OFFSET + WSAEINTR ? 1 : 0;
#else
	return errno == EINTR ? 1 : 0;
#endif
}

int errno_would_block(void) {
#ifdef _WIN32
	return errno == ERRNO_WINAPI_OFFSET + WSAEWOULDBLOCK ? 1 : 0;
#else
	return errno == EWOULDBLOCK || errno == EAGAIN ? 1 : 0;
#endif
}

const char *get_errno_name(int error_code) {
	#define ERRNO_NAME(code) case code: return #code
	#define WINAPI_ERROR_NAME(code) case ERRNO_WINAPI_OFFSET + code: return #code
#ifndef _WIN32
	#if EAI_AGAIN < 0
		#define ADDRINFO_ERROR_NAME(code) case ERRNO_ADDRINFO_OFFSET - code: return #code
	#else
		#define ADDRINFO_ERROR_NAME(code) case ERRNO_ADDRINFO_OFFSET + code: return #code
	#endif
#endif

	switch (error_code) {
	ERRNO_NAME(EPERM);
	ERRNO_NAME(ENOENT);
	ERRNO_NAME(ESRCH);
	ERRNO_NAME(EINTR);
	ERRNO_NAME(EIO);
	ERRNO_NAME(ENXIO);
	ERRNO_NAME(E2BIG);
	ERRNO_NAME(ENOEXEC);
	ERRNO_NAME(EBADF);
	ERRNO_NAME(ECHILD);
	ERRNO_NAME(EAGAIN);
	ERRNO_NAME(ENOMEM);
	ERRNO_NAME(EACCES);
	ERRNO_NAME(EFAULT);
#ifdef ENOTBLK
	ERRNO_NAME(ENOTBLK);
#endif
	ERRNO_NAME(EBUSY);
	ERRNO_NAME(EEXIST);
	ERRNO_NAME(EXDEV);
	ERRNO_NAME(ENODEV);
	ERRNO_NAME(ENOTDIR);
	ERRNO_NAME(EISDIR);
	ERRNO_NAME(EINVAL);
	ERRNO_NAME(ENFILE);
	ERRNO_NAME(EMFILE);
	ERRNO_NAME(ENOTTY);
#ifdef ETXTBSY
	ERRNO_NAME(ETXTBSY);
#endif
	ERRNO_NAME(EFBIG);
	ERRNO_NAME(ENOSPC);
	ERRNO_NAME(ESPIPE);
	ERRNO_NAME(EROFS);
	ERRNO_NAME(EMLINK);
	ERRNO_NAME(EPIPE);
	ERRNO_NAME(EDOM);
	ERRNO_NAME(ERANGE);
	ERRNO_NAME(EDEADLK);
	ERRNO_NAME(ENAMETOOLONG);
	ERRNO_NAME(ENOLCK);
	ERRNO_NAME(ENOSYS);
	ERRNO_NAME(ENOTEMPTY);

#ifndef _WIN32
	ERRNO_NAME(ELOOP);
	#if EWOULDBLOCK != EAGAIN
	ERRNO_NAME(EWOULDBLOCK);
	#endif
	ERRNO_NAME(ENOMSG);
	ERRNO_NAME(EIDRM);
	ERRNO_NAME(ENOSTR);
	ERRNO_NAME(ENODATA);
	ERRNO_NAME(ETIME);
	ERRNO_NAME(ENOSR);
	ERRNO_NAME(EREMOTE);
	ERRNO_NAME(ENOLINK);
	ERRNO_NAME(EPROTO);
	ERRNO_NAME(EMULTIHOP);
	ERRNO_NAME(EBADMSG);
	ERRNO_NAME(EOVERFLOW);
	ERRNO_NAME(EUSERS);
	ERRNO_NAME(ENOTSOCK);
	ERRNO_NAME(EDESTADDRREQ);
	ERRNO_NAME(EMSGSIZE);
	ERRNO_NAME(EPROTOTYPE);
	ERRNO_NAME(ENOPROTOOPT);
	ERRNO_NAME(EPROTONOSUPPORT);
	ERRNO_NAME(ESOCKTNOSUPPORT);
	ERRNO_NAME(EOPNOTSUPP);
	ERRNO_NAME(EPFNOSUPPORT);
	ERRNO_NAME(EAFNOSUPPORT);
	ERRNO_NAME(EADDRINUSE);
	ERRNO_NAME(EADDRNOTAVAIL);
	ERRNO_NAME(ENETDOWN);
	ERRNO_NAME(ENETUNREACH);
	ERRNO_NAME(ENETRESET);
	ERRNO_NAME(ECONNABORTED);
	ERRNO_NAME(ECONNRESET);
	ERRNO_NAME(ENOBUFS);
	ERRNO_NAME(EISCONN);
	ERRNO_NAME(ENOTCONN);
	ERRNO_NAME(ESHUTDOWN);
	ERRNO_NAME(ETOOMANYREFS);
	ERRNO_NAME(ETIMEDOUT);
	ERRNO_NAME(ECONNREFUSED);
	ERRNO_NAME(EHOSTDOWN);
	ERRNO_NAME(EHOSTUNREACH);
	ERRNO_NAME(EALREADY);
	ERRNO_NAME(EINPROGRESS);
	ERRNO_NAME(ESTALE);
	ERRNO_NAME(EDQUOT);
	ERRNO_NAME(ECANCELED);
	ERRNO_NAME(EOWNERDEAD);
	ERRNO_NAME(ENOTRECOVERABLE);
#endif

#if !defined _WIN32 && !defined __APPLE__
	ERRNO_NAME(ECHRNG);
	ERRNO_NAME(EL2NSYNC);
	ERRNO_NAME(EL3HLT);
	ERRNO_NAME(EL3RST);
	ERRNO_NAME(ELNRNG);
	ERRNO_NAME(EUNATCH);
	ERRNO_NAME(ENOCSI);
	ERRNO_NAME(EL2HLT);
	ERRNO_NAME(EBADE);
	ERRNO_NAME(EBADR);
	ERRNO_NAME(EXFULL);
	ERRNO_NAME(ENOANO);
	ERRNO_NAME(EBADRQC);
	ERRNO_NAME(EBADSLT);
	#if EDEADLOCK != EDEADLK
	ERRNO_NAME(EDEADLOCK);
	#endif
	ERRNO_NAME(EBFONT);
	ERRNO_NAME(ENONET);
	ERRNO_NAME(ENOPKG);
	ERRNO_NAME(EADV);
	ERRNO_NAME(ESRMNT);
	ERRNO_NAME(ECOMM);
	ERRNO_NAME(EDOTDOT);
	ERRNO_NAME(ENOTUNIQ);
	ERRNO_NAME(EBADFD);
	ERRNO_NAME(EREMCHG);
	ERRNO_NAME(ELIBACC);
	ERRNO_NAME(ELIBBAD);
	ERRNO_NAME(ELIBSCN);
	ERRNO_NAME(ELIBMAX);
	ERRNO_NAME(ELIBEXEC);
	ERRNO_NAME(EILSEQ);
	ERRNO_NAME(ERESTART);
	ERRNO_NAME(ESTRPIPE);
	ERRNO_NAME(EUCLEAN);
	ERRNO_NAME(ENOTNAM);
	ERRNO_NAME(ENAVAIL);
	ERRNO_NAME(EISNAM);
	ERRNO_NAME(EREMOTEIO);
	ERRNO_NAME(ENOMEDIUM);
	ERRNO_NAME(EMEDIUMTYPE);
	ERRNO_NAME(ENOKEY);
	ERRNO_NAME(EKEYEXPIRED);
	ERRNO_NAME(EKEYREVOKED);
	ERRNO_NAME(EKEYREJECTED);
	#ifdef ERFKILL
	ERRNO_NAME(ERFKILL);
	#endif
#endif

#ifdef _WIN32
	WINAPI_ERROR_NAME(ERROR_FAILED_SERVICE_CONTROLLER_CONNECT);
	WINAPI_ERROR_NAME(ERROR_INVALID_DATA);
	WINAPI_ERROR_NAME(ERROR_ACCESS_DENIED);
	WINAPI_ERROR_NAME(ERROR_INVALID_HANDLE);
	WINAPI_ERROR_NAME(ERROR_INVALID_NAME);
	WINAPI_ERROR_NAME(ERROR_CIRCULAR_DEPENDENCY);
	WINAPI_ERROR_NAME(ERROR_INVALID_PARAMETER);
	WINAPI_ERROR_NAME(ERROR_INVALID_SERVICE_ACCOUNT);
	WINAPI_ERROR_NAME(ERROR_DUPLICATE_SERVICE_NAME);
	WINAPI_ERROR_NAME(ERROR_SERVICE_ALREADY_RUNNING);
	WINAPI_ERROR_NAME(ERROR_SERVICE_DOES_NOT_EXIST);
	WINAPI_ERROR_NAME(ERROR_SERVICE_EXISTS);
	WINAPI_ERROR_NAME(ERROR_SERVICE_MARKED_FOR_DELETE);
	WINAPI_ERROR_NAME(ERROR_INSUFFICIENT_BUFFER);
	WINAPI_ERROR_NAME(ERROR_INVALID_WINDOW_HANDLE);
	WINAPI_ERROR_NAME(ERROR_ALREADY_EXISTS);
	WINAPI_ERROR_NAME(ERROR_FILE_NOT_FOUND);
	WINAPI_ERROR_NAME(ERROR_INVALID_SERVICE_CONTROL);
	WINAPI_ERROR_NAME(ERROR_OPERATION_ABORTED);
	WINAPI_ERROR_NAME(ERROR_IO_INCOMPLETE);
	WINAPI_ERROR_NAME(ERROR_IO_PENDING);
	WINAPI_ERROR_NAME(ERROR_PIPE_BUSY);

	WINAPI_ERROR_NAME(WSAEINTR);
	WINAPI_ERROR_NAME(WSAEBADF);
	WINAPI_ERROR_NAME(WSAEACCES);
	WINAPI_ERROR_NAME(WSAEFAULT);
	WINAPI_ERROR_NAME(WSAEINVAL);
	WINAPI_ERROR_NAME(WSAEMFILE);
	WINAPI_ERROR_NAME(WSAEWOULDBLOCK);
	WINAPI_ERROR_NAME(WSAEINPROGRESS);
	WINAPI_ERROR_NAME(WSAEALREADY);
	WINAPI_ERROR_NAME(WSAENOTSOCK);
	WINAPI_ERROR_NAME(WSAEDESTADDRREQ);
	WINAPI_ERROR_NAME(WSAEMSGSIZE);
	WINAPI_ERROR_NAME(WSAEPROTOTYPE);
	WINAPI_ERROR_NAME(WSAENOPROTOOPT);
	WINAPI_ERROR_NAME(WSAEPROTONOSUPPORT);
	WINAPI_ERROR_NAME(WSAESOCKTNOSUPPORT);
	WINAPI_ERROR_NAME(WSAEOPNOTSUPP);
	WINAPI_ERROR_NAME(WSAEPFNOSUPPORT);
	WINAPI_ERROR_NAME(WSAEAFNOSUPPORT);
	WINAPI_ERROR_NAME(WSAEADDRINUSE);
	WINAPI_ERROR_NAME(WSAEADDRNOTAVAIL);
	WINAPI_ERROR_NAME(WSAENETDOWN);
	WINAPI_ERROR_NAME(WSAENETUNREACH);
	WINAPI_ERROR_NAME(WSAENETRESET);
	WINAPI_ERROR_NAME(WSAECONNABORTED);
	WINAPI_ERROR_NAME(WSAECONNRESET);
	WINAPI_ERROR_NAME(WSAENOBUFS);
	WINAPI_ERROR_NAME(WSAEISCONN);
	WINAPI_ERROR_NAME(WSAENOTCONN);
	WINAPI_ERROR_NAME(WSAESHUTDOWN);
	WINAPI_ERROR_NAME(WSAETOOMANYREFS);
	WINAPI_ERROR_NAME(WSAETIMEDOUT);
	WINAPI_ERROR_NAME(WSAECONNREFUSED);
	WINAPI_ERROR_NAME(WSAELOOP);
	WINAPI_ERROR_NAME(WSAENAMETOOLONG);
	WINAPI_ERROR_NAME(WSAEHOSTDOWN);
	WINAPI_ERROR_NAME(WSAEHOSTUNREACH);
	WINAPI_ERROR_NAME(WSAENOTEMPTY);
	WINAPI_ERROR_NAME(WSAEPROCLIM);
	WINAPI_ERROR_NAME(WSAEUSERS);
	WINAPI_ERROR_NAME(WSAEDQUOT);
	WINAPI_ERROR_NAME(WSAESTALE);
	WINAPI_ERROR_NAME(WSAEREMOTE);

	WINAPI_ERROR_NAME(WSATRY_AGAIN);
	WINAPI_ERROR_NAME(WSANO_RECOVERY);
	WINAPI_ERROR_NAME(WSA_NOT_ENOUGH_MEMORY);
	WINAPI_ERROR_NAME(WSAHOST_NOT_FOUND);
#endif

#ifndef _WIN32
	ADDRINFO_ERROR_NAME(EAI_AGAIN);
	ADDRINFO_ERROR_NAME(EAI_BADFLAGS);
	ADDRINFO_ERROR_NAME(EAI_FAIL);
	ADDRINFO_ERROR_NAME(EAI_FAMILY);
	ADDRINFO_ERROR_NAME(EAI_MEMORY);
	ADDRINFO_ERROR_NAME(EAI_NONAME);
	ADDRINFO_ERROR_NAME(EAI_OVERFLOW);
	ADDRINFO_ERROR_NAME(EAI_SYSTEM);
	ADDRINFO_ERROR_NAME(EAI_ADDRFAMILY);
#endif

	// FIXME

	default: return "<unknown>";
	}

	#undef ERRNO_NAME
	#undef WINAPI_ERROR_NAME
	#undef ADDRINFO_ERROR_NAME
}

void string_copy(char *destination, const char *source, int size) {
	if (size <= 0) {
		return;
	}

	strncpy(destination, source, size - 1);

	destination[size - 1] = '\0';
}

void string_append(char *destination, const char *source, int size) {
	int offset;

	if (size <= 0) {
		return;
	}

	offset = strlen(destination);

	if (offset >= size - 1) {
		return;
	}

	strncpy(destination + offset, source, size - offset - 1);

	destination[size - 1] = '\0';
}

static const char *base58_alphabet = "123456789abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNPQRSTUVWXYZ";

char *base58_encode(char *base58, uint32_t value) {
	uint32_t digit;
	char reverse[BASE58_MAX_LENGTH];
	int i = 0;
	int k = 0;

	while (value >= 58) {
		digit = value % 58;
		reverse[i] = base58_alphabet[digit];
		value = value / 58;
		++i;
	}

	reverse[i] = base58_alphabet[value];

	for (k = 0; k <= i; ++k) {
		base58[k] = reverse[i - k];
	}

	for (; k < BASE58_MAX_LENGTH; ++k) {
		base58[k] = '\0';
	}

	return base58;
}

// sets errno on error
int base58_decode(uint32_t *value, const char *base58) {
	int i;
	const char *p;
	int k;
	uint32_t base = 1;

	*value = 0;
	i = strlen(base58) - 1;

	if (i < 0) {
		errno = EINVAL;

		return -1;
	}

	for (; i >= 0; --i) {
		p = strchr(base58_alphabet, base58[i]);

		if (p == NULL) {
			errno = EINVAL;

			return -1;
		}

		k = p - base58_alphabet;

		if (*value > UINT32_MAX - k * base) {
			errno = ERANGE;

			return -1;
		}

		*value += k * base;
		base *= 58;
	}

	return 0;
}

// convert from host endian to little endian
uint16_t uint16_to_le(uint16_t native) {
	union {
		uint8_t bytes[2];
		uint16_t little;
	} c;

	c.bytes[0] = (native >> 0) & 0xFF;
	c.bytes[1] = (native >> 8) & 0xFF;

	return c.little;
}

// convert from host endian to little endian
uint32_t uint32_to_le(uint32_t native) {
	union {
		uint8_t bytes[4];
		uint32_t little;
	} c;

	c.bytes[0] = (native >>  0) & 0xFF;
	c.bytes[1] = (native >>  8) & 0xFF;
	c.bytes[2] = (native >> 16) & 0xFF;
	c.bytes[3] = (native >> 24) & 0xFF;

	return c.little;
}

// convert from little endian to host endian
uint32_t uint32_from_le(uint32_t value) {
	uint8_t *bytes = (uint8_t *)&value;

	return ((uint32_t)bytes[3] << 24) |
	       ((uint32_t)bytes[2] << 16) |
	       ((uint32_t)bytes[1] <<  8) |
	       ((uint32_t)bytes[0] <<  0);
}

void millisleep(uint32_t milliseconds) {
#ifdef _WIN32
	Sleep(milliseconds);
#else
	// FIXME: (u)sleep can be interrupted, might have to deal with that
	if (milliseconds >= 1000) {
		sleep(milliseconds / 1000);

		milliseconds %= 1000;
	}

	usleep(milliseconds * 1000);
#endif
}

uint64_t microseconds(void) {
	struct timeval tv;

	// FIXME: use a monotonic source such as clock_gettime(CLOCK_MONOTONIC),
	//        QueryPerformanceCounter() or mach_absolute_time()
	if (gettimeofday(&tv, NULL) < 0) {
		return 0;
	} else {
		return tv.tv_sec * 1000000 + tv.tv_usec;
	}
}

#if !defined _GNU_SOURCE && !defined __APPLE__

#include <ctype.h>

char *strcasestr(char *haystack, char *needle) {
	char *p, *startn = NULL, *np = NULL;

	for (p = haystack; *p != '\0'; ++p) {
		if (np != NULL) {
			if (toupper(*p) == toupper(*np)) {
				if (*++np == '\0') {
					return startn;
				}
			} else {
				np = NULL;
			}
		} else if (toupper(*p) == toupper(*needle)) {
			np = needle + 1;
			startn = p;
		}
	}

	return NULL;
}

#endif

#define RED_BRICK_UID_FILENAME "/proc/red_brick_uid"

// sets errno on error
int red_brick_uid(uint32_t *uid /* always little endian */) {
	FILE *fp;
	char base58[BASE58_MAX_LENGTH + 1]; // +1 for the \n
	int rc;
	int saved_errno;

	// read UID from /proc/red_brick_uid
	fp = fopen(RED_BRICK_UID_FILENAME, "rb");

	if (fp == NULL) {
		return -1;
	}

	rc = fread(base58, 1, sizeof(base58), fp);
	saved_errno = errno;

	fclose(fp);

	errno = saved_errno;

	if (rc < 1) {
		return -1;
	}

	if (base58[rc - 1] != '\n') {
		errno = EINVAL;

		return -1;
	}

	base58[rc - 1] = '\0';

	if (base58_decode(uid, base58) < 0) {
		return -1;
	}

	*uid = uint32_to_le(*uid);

	return 0;
}