// In C++, the `socket` function is part of the socket programming interface,
// which is used to create a new socket. Sockets are fundamental building blocks
// for network communication, enabling processes on different devices to
// communicate over a network.

// Here's a brief overview of how the `socket` function works in C++:

// 1. **Including Necessary Headers:**
// 	To use socket-related functions in C++, you typically include the
// 	`<sys/socket.h>` header file.

// 2. **Creating a Socket:**
// 	The `socket` function is used to create a socket with a specific communication
// 	domain, socket type, and protocol.

// Syntax:
// int socket(int domain, int type, int protocol);

// - `domain`: Specifies the communication domain or address family
// 	(such as `AF_INET` for IPv4, `AF_INET6` for IPv6, etc.).
// - `type`: Specifies the socket type (e.g., `SOCK_STREAM` for TCP sockets, 
// 	SOCK_DGRAM` for UDP sockets, etc.).
// - `protocol`: Indicates the specific protocol to be used
// 	(usually `0` for the default protocol associated with the given socket type).

// 3. **Handling Errors:**
// 	The function returns a file descriptor (a non-negative integer) representing
// 	the socket on success. On failure, it returns `-1`,
// 	indicating an error occurred during socket creation. You should check the
// 	return value to handle errors appropriately.

// 4. **Using the Created Socket:**
// 	Once a socket is created, it can be used in conjunction with other socket-related
// 	functions, such as `bind`, `connect`, `listen`, `send`, `recv`, etc.,
// 	to establish network connections, send and receive data, and manage communication
// 	over the network.

// Here's a simple example illustrating the usage of `socket` to create a TCP/IP socket:

#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

int main() {
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == -1) {
		std::cerr << "Error creating socket\n";
		return 1;
	}

	// Use the created socket for further operations like binding, connecting, etc.

	// Close the socket when done
	close(serverSocket);

	return 0;
}

// Remember, after creating a socket and using it for communication,
// it's important to properly manage the socket by closing it when
// it's no longer needed to free up system resources.