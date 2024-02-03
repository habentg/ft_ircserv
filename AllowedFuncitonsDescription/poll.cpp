/* `poll` is a system call in C used for monitoring multiple file descriptors
to see if I/O is possible on any of them. It's a way to wait for events on
multiple file descriptors, similar to `select` and `epoll` on Linux systems.

Here's a basic example of using `poll` to check for events on a single
file descriptor: */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/poll.h>

int main() {
	struct pollfd fds[1];
	int timeout_ms = 5000; // Timeout in milliseconds

	fds[0].fd = STDIN_FILENO; // File descriptor for stdin
	fds[0].events = POLLIN;   // Interested in reading

	int ret = poll(fds, 1, timeout_ms);

	if (ret == -1) {
		perror("poll");
		exit(EXIT_FAILURE);
	} else if (ret == 0) {
		printf("Timeout occurred!\n");
	} else {
		if (fds[0].revents & POLLIN) {
			printf("Data available on stdin\n");
		}
	}

	return 0;
}

/* This code initializes a `pollfd` structure with information about a
single file descriptor (`STDIN_FILENO` for standard input), specifies
that it's interested in reading (`POLLIN`), and then calls `poll` to
wait for an event on that descriptor. The code checks if the event was
triggered by checking the `revents` member of the `pollfd` structure.

Keep in mind that `poll` can monitor multiple file descriptors simultaneously
by using an array of `pollfd` structures. The function will return when an event
occurs on any of the monitored descriptors or when the timeout expires. */