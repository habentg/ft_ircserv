// The `select` function in C is used for monitoring multiple file
// descriptors to see if I/O is possible on any of them. It allows
// you to wait until one or more of the specified file descriptors
// become "ready" for some class of I/O operation (e.g., read, write,
// or an exceptional condition).

// Here's a basic example of how `select` can be used:

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>

int main() {
	fd_set readfds;
	struct timeval timeout;

	FD_ZERO(&readfds);
	FD_SET(STDIN_FILENO, &readfds); // Add stdin to the set

	timeout.tv_sec = 5; // Timeout in seconds
	timeout.tv_usec = 0; // No microseconds

	int ret = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

	if (ret == -1) {
		perror("select");
		exit(EXIT_FAILURE);
	} else if (ret == 0) {
		printf("Timeout occurred!\n");
	} else {
		if (FD_ISSET(STDIN_FILENO, &readfds)) {
			printf("Data available on stdin\n");
		}
	}

	return 0;
}

// In this example:
// - `FD_ZERO` initializes the set of file descriptors to zero.
// - `FD_SET` adds the standard input (file descriptor `STDIN_FILENO`)
// to the set.
// - `select` is called with the set of file descriptors (`readfds`),
// specifying a timeout of 5 seconds.
// - The function will return when either the timeout expires or when
// input becomes available on the specified file descriptor.

// Keep in mind that `select` has some limitations, such as handling a
// maximum number of file descriptors and being less efficient compared
// to `poll` or `epoll` for larger-scale applications with many file descriptors.