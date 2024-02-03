/* The `bind` function in C/C++ is used in network programming, especially
with sockets, to associate a socket with a specific address (such as an IP address)
and a port number on the local machine. Here's what it does:

1. **Binding a Socket:** When you create a socket using functions
like `socket`, it exists in a nameless state. To use it, you need
to bind it to a specific address and port.

2. **Specifying the Local Endpoint:** `bind` assigns the socket a
local address, allowing it to listen for incoming connections or
to specify where data should be sent from this socket.

3. **For Server Programs:** In server applications, `bind` is
typically used to specify the port and address that clients will connect to.

Here's an example of using `bind`: */

#include <stdio.h> //printf
#include <stdlib.h> //EXIT_FAILURE!
#include <string.h> //memset
#include <sys/socket.h> // socket
#include <netinet/in.h> // sockaddr_in struct!

#define PORT 8080

int main() {
	int sockfd;
	struct sockaddr_in servaddr;

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

	// Binding the socket with the server address
	if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	printf("Socket bound successfully\n");

	// ... Other code to listen, accept connections, etc.

	return 0;
}

/* In this example, `bind` associates the socket `sockfd` with the address
and port specified in `servaddr`. After successfully binding the socket,
it is ready to be used, such as for listening to incoming connections in
a server program. */