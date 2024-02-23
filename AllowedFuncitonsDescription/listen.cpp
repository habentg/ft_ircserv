/* The `listen` function in C/C++ is used in server applications to set
up a socket for incoming connections. Specifically:

1. **Server Socket Setup:** After creating a socket using `socket`,
and before handling incoming connections using `accept`, you use `listen`
to prepare the socket for incoming connections.

2. **Backlog Size:** It defines the maximum number of pending connections
that can be queued for acceptance. When the maximum is reached, additional
connection attempts may be refused by the operating system.

Here's an example of using `listen`:
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h> // INADDR_ANY, struct sockaddr_in

#define PORT 8080
#define MAX_PENDING_CONNECTIONS 10

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

	// ... Other code for accepting incoming connections

	return 0;
}

/* In this example, `listen` prepares the server socket (`sockfd`) to accept
incoming connections. It sets up a queue where incoming connection requests
will wait until the server accepts them using `accept`. The `MAX_PENDING_CONNECTIONS`
parameter determines the size of this backlog queue for pending connections. */