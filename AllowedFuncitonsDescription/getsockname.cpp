// The `getsockname` function in C and C++ retrieves the local address and local port of a bound socket.
// It provides information about the locally bound address and port number associated with a specified socket.

// The syntax of the `getsockname` function is as follows:

// ```cpp
// int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
// ```

// Parameters:
// - `sockfd`: The file descriptor of the socket for which you want to retrieve the local address and port.
// - `addr`: A pointer to a `struct sockaddr` where the address information will be stored.
// - `addrlen`: A pointer to a `socklen_t` variable containing the size of the buffer pointed to by `addr`.
// On output, it contains the actual size of the address stored in `addr`.

// The `getsockname` function is typically used after a socket has been bound to an address and port using
// `bind` to determine which local address and port the socket is using. This can be useful, for example,
// when the operating system assigns a port number when binding a socket to an address.

// Here's a simple example demonstrating the use of `getsockname`:

#include <iostream>
#include <unistd.h> // close
#include <arpa/inet.h> // inet_ntop
// #include <sys/socket.h>
// #include <netinet/in.h>

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "Error creating socket\n";
		return 1;
	}

	struct sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(8080); // Port number 8080

	// Bind the socket
	if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
		std::cerr << "Error binding socket\n";
		close(sockfd);
		return 1;
	}

	// Retrieve local address and port information
	struct sockaddr_in localAddr;
	socklen_t addrLen = sizeof(localAddr);
	if (getsockname(sockfd, (struct sockaddr *)&localAddr, &addrLen) == -1) {
		std::cerr << "Error getting socket name\n";
		close(sockfd);
		return 1;
	}

	// Convert address to human-readable format (if needed)
	char ipAddr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(localAddr.sin_addr), ipAddr, INET_ADDRSTRLEN);

	std::cout << "Local address: " << ipAddr << std::endl;
	std::cout << "Local port: " << ntohs(localAddr.sin_port) << std::endl;

	// Close the socket when done
	close(sockfd);

	return 0;
}

// This example creates a socket, binds it to port 8080 on any available local address, and then
// uses `getsockname` to retrieve and display the local address and port to which the socket is bound.