

/* The `accept` function in C/C++ is used in server-side network programming
to accept incoming client connections on a socket that's been set up and
put into a listening state using `listen`. 

Once a client attempts to establish a connection to the server's listening
socket, `accept` is used to accept that connection request, creating a new
socket specifically for communication with that client.

Here's an example: */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>

#define PORT 8080
#define MAX_PENDING_CONNECTIONS 10

int main() {
	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding socket to the specified address and port
	if ((bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr))) != 0) {
		perror("socket bind failed");
		exit(EXIT_FAILURE);
	}

	// Listen for incoming connections
	if ((listen(sockfd, MAX_PENDING_CONNECTIONS)) != 0) {
		perror("listen failed");
		exit(EXIT_FAILURE);
	}

	printf("Server listening on port %d\n", PORT);

	// Accepting incoming connection
	socklen_t len = sizeof(cli);
	connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
	if (connfd < 0) {
		perror("server accept failed");
		exit(EXIT_FAILURE);
	}

	printf("Server accepted client connection\n");

	// ... Communication with the client using 'connfd'

	close(sockfd); // Closing the server socket

	return 0;
}

/* Here, after the server sets up its listening socket using `listen`, the
`accept` function is used to accept an incoming client connection. Once
the connection is accepted, it returns a new socket file descriptor (`connfd`)
that can be used to communicate with that specific client. */