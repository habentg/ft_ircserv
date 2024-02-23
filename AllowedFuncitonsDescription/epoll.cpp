/* `epoll` is an I/O event notification facility in Linux designed to
overcome some limitations of older mechanisms like `select` and `poll`
when handling a large number of file descriptors.

Key aspects of `epoll`:

1. **Scalability**: Highly scalable and efficient for handling a
large number of file descriptors or events.
2. **Event-driven**: It provides an event notification mechanism
to notify when I/O operations are ready on file descriptors.
3. **Three System Calls**: There are three primary system calls
associated with `epoll`:
   - `epoll_create`: Creates an epoll instance and returns a
   file descriptor.
   - `epoll_ctl`: Controls which file descriptors to monitor
   and what events to monitor for.
   - `epoll_wait`: Waits for events on the file descriptors
   registered with the epoll instance.

The typical workflow involves creating an epoll instance,
adding file descriptors to it, waiting for events, and
handling the events when they occur.

Here's a basic example demonstrating the use of `epoll` to
monitor file descriptor events: */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <fcntl.h>

#define MAX_EVENTS 10

int main() {
	int epoll_fd, file_descriptor;
	struct epoll_event event, events[MAX_EVENTS];

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

	file_descriptor = open("example.txt", O_RDONLY);
	if (file_descriptor == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	event.events = EPOLLIN;
	event.data.fd = file_descriptor;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, file_descriptor, &event) == -1) {
		perror("epoll_ctl");
		exit(EXIT_FAILURE);
	}

	printf("Monitoring 'example.txt' for changes. Press enter to exit.\n");

	int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
	if (num_events == -1) {
		perror("epoll_wait");
		exit(EXIT_FAILURE);
	}

	printf("Event detected!\n");

	close(file_descriptor);
	close(epoll_fd);

	return 0;
}

/* This example demonstrates how to use `epoll` to monitor changes in a
file (`example.txt`). It creates an epoll instance, adds a file descriptor
for monitoring, waits for events, and handles the events when they occur.
In this case, it waits indefinitely until an event occurs on the monitored
file descriptor.

The actual use of `epoll` involves handling various events, error checking,
and managing multiple file descriptors efficiently, making it a powerful tool
for scalable I/O event handling in Linux. */