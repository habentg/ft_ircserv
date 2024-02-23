/* The `fcntl` function in C is used for performing various control
operations on open files. It can be used to perform tasks like
duplicating a file descriptor, changing file descriptor properties,
or performing other control operations on open file descriptors.

Here's a simple example demonstrating the use of `fcntl` to change
the file descriptor flags: */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main() {
	int fd;
	int flags;

	fd = open("example.txt", O_RDWR);
	if (fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	// Get current file status flags
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		perror("fcntl");
		close(fd);
		exit(EXIT_FAILURE);
	}

	printf("Before fcntl, file status flags: %d\n", flags);

	// Add additional flags (O_APPEND)
	flags |= O_APPEND;

	// Set modified flags using fcntl
	if (fcntl(fd, F_SETFL, flags) == -1) {
		perror("fcntl");
		close(fd);
		exit(EXIT_FAILURE);
	}

	// Get updated file status flags
	if ((flags = fcntl(fd, F_GETFL, 0)) == -1) {
		perror("fcntl");
		close(fd);
		exit(EXIT_FAILURE);
	}

	printf("After fcntl, file status flags: %d\n", flags);

	close(fd);
	return 0;
}

/* This example opens a file, retrieves its current file status flags
using `fcntl` with `F_GETFL`, then modifies the flags by adding `O_APPEND`.
Finally, it prints the updated flags using `fcntl` with `F_SETFL`. */