/* The `connect` function in C/C++ is used to establish a connection to a
remote endpoint, typically in a client-server model. Here's what it does:

1. **Client-side Connection:** For client applications, after creating a
socket using `socket`, you use `connect` to establish a connection to a
server listening on a specific address and port.

2. **Specifying the Remote Endpoint:** `connect` is used to specify the
address and port of the remote server to which you want to establish a connection.

3. **Establishing a Connection:** It initiates a three-way handshake (SYN, SYN-ACK, ACK)
 to establish a reliable connection for subsequent communication.

Here's an example of using `connect`: */

#include <stdio.h> // printf, perror
#include <stdlib.h> // htons, EXIT_FAILURE
#include <arpa/inet.h>// inet_addr, struct sockaddr_in, AF_INET, SOCK_STREAM, htons

#define PORT 8080
#define SERVER_IP "127.0.0.1"  // Example IP address

int main() {
	int sockfd;
	struct sockaddr_in servaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	// memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servaddr.sin_port = htons(PORT);

	// Connecting to the server
	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) {
		perror("connection failed");
		exit(EXIT_FAILURE);
	}

	printf("Connected to server\n");

	// ... Other code for sending/receiving data

	return 0;
}

/* In this example, `connect` attempts to establish a connection from the client
to the server specified by `SERVER_IP` and `PORT`. Upon successful execution,
it indicates that the connection has been established and the client can send
and receive data to/from the server. */