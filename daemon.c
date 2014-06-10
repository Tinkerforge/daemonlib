/*
 * daemonlib
 * Copyright (C) 2012-2014 Matthias Bolte <matthias@tinkerforge.com>
 *
 * daemon.c: Daemon implementation
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
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pwd.h>
#include <unistd.h>
#include <fcntl.h>

#include "daemon.h"

#include "log.h"
#include "pid_file.h"
#include "utils.h"

#define LOG_CATEGORY LOG_CATEGORY_OTHER

static int daemon_parent(pid_t child, int status_read, const char *pid_filename) {
	int8_t status = -1;
	ssize_t rc;

	// wait for first child to exit
	while (waitpid(child, NULL, 0) < 0 && errno == EINTR) {
	}

	/*if (waitpid(pid, NULL, 0) < 0) {
		fprintf(stderr, "Could not wait for first child process to exit: %s (%d)\n",
		        get_errno_name(errno), errno);

		close(status_pipe[0]);

		return -1;
	}*/

	// wait for second child to start successfully
	while ((rc = read(status_read, &status, 1)) < 0 && errno == EINTR) {
	}

	if (status < 0) {
		if (rc < 0) {
			fprintf(stderr, "Could not read from status pipe: %s (%d)\n",
			        get_errno_name(errno), errno);
		}

		close(status_read);

		exit(EXIT_FAILURE);
	}

	close(status_read);

	if (status != 1) {
		if (status == 2) {
			fprintf(stderr, "Already running according to '%s'\n", pid_filename);
		} else {
			fprintf(stderr, "Second child process exited with an error (status: %d)\n",
			        status);
		}

		exit(EXIT_FAILURE);
	}

	// exit first parent
	exit(EXIT_SUCCESS);
}

int daemon_start_double_fork(const char *log_filename, const char *pid_filename) {
	int status_pipe[2];
	pid_t pid;
	int pid_fd = -1;
	int8_t status = 0;
	FILE *log_file;
	int stdin_fd = -1;
	int stdout_fd = -1;

	// create status pipe
	if (pipe(status_pipe) < 0) {
		fprintf(stderr, "Could not create status pipe: %s (%d)\n",
		        get_errno_name(errno), errno);

		return -1;
	}

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Could not fork first child process: %s (%d)\n",
		        get_errno_name(errno), errno);

		close(status_pipe[0]);
		close(status_pipe[1]);

		return -1;
	}

	if (pid > 0) { // first parent
		close(status_pipe[1]);

		daemon_parent(pid, status_pipe[0], pid_filename);
	}

	// first child, decouple from parent environment
	close(status_pipe[0]);

	if (chdir("/") < 0) {
		fprintf(stderr, "Could not change directory to '/': %s (%d)\n",
		        get_errno_name(errno), errno);

		close(status_pipe[1]);

		exit(EXIT_FAILURE);
	}

	// FIXME: check error
	setsid();
	umask(0);

	pid = fork();

	if (pid < 0) {
		fprintf(stderr, "Could not fork second child process: %s (%d)\n",
		        get_errno_name(errno), errno);

		close(status_pipe[1]);

		exit(EXIT_FAILURE);
	}

	if (pid > 0) {
		// exit second parent
		exit(EXIT_SUCCESS);
	}

	// second child, write pid
	pid_fd = pid_file_acquire(pid_filename, getpid());

	if (pid_fd < 0) {
		if (pid_fd < -1) {
			status = 2;
		}

		goto cleanup;
	}

	// open log file
	log_file = fopen(log_filename, "a+");

	if (log_file == NULL) {
		fprintf(stderr, "Could not open log file '%s': %s (%d)\n",
		        log_filename, get_errno_name(errno), errno);

		goto cleanup;
	}

	log_set_file(log_file);

	// redirect standard file descriptors
	stdin_fd = open("/dev/null", O_RDONLY);

	if (stdin_fd < 0) {
		fprintf(stderr, "Could not open /dev/null to redirect stdin to: %s (%d)\n",
		        get_errno_name(errno), errno);

		goto cleanup;
	}

	stdout_fd = fileno(log_file);

	if (dup2(stdin_fd, STDIN_FILENO) != STDIN_FILENO) {
		fprintf(stderr, "Could not redirect stdin: %s (%d)\n",
		        get_errno_name(errno), errno);

		goto cleanup;
	}

	if (dup2(stdout_fd, STDOUT_FILENO) != STDOUT_FILENO) {
		fprintf(stderr, "Could not redirect stdout: %s (%d)\n",
		        get_errno_name(errno), errno);

		goto cleanup;
	}

	if (dup2(stdout_fd, STDERR_FILENO) != STDERR_FILENO) {
		fprintf(stderr, "Could not redirect stderr: %s (%d)\n",
		        get_errno_name(errno), errno);

		goto cleanup;
	}

	status = 1;

cleanup:
	if (stdin_fd > STDERR_FILENO) {
		close(stdin_fd);
	}

	while (write(status_pipe[1], &status, 1) < 0 && errno == EINTR) {
	}

	close(status_pipe[1]);

	return status == 1 ? pid_fd : -1;
}