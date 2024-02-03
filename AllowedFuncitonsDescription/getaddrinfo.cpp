
// The `getaddrinfo` function in C and C++ is used to perform hostname to IP address resolution,
// as well as service name to port number resolution. It's a more versatile and modern function
// compared to `gethostbyname` or `gethostbyaddr` as it supports both IPv4 and IPv6, protocol
// independence, and provides better error handling capabilities.

// Syntax:
// ```c
// int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **res);
// ```

// - `node`: Specifies the hostname or IP address for which you want to resolve information.
// It can be a hostname, an IPv4/IPv6 address in numeric string format, or `NULL` to indicate the local host.
  
// - `service`: Specifies the service name or port number associated with the desired service.
// It can be a service name (e.g., "http", "ssh", etc.) or a decimal port number in a string format.

// - `hints`: A pointer to a struct `addrinfo` that specifies additional options and hints for
// the query. It can be set to `NULL` if no specific hints are required.

// - `res`: A pointer to a pointer to `struct addrinfo`, which is filled with the result of
// the query containing a linked list of address structures. This result should be freed using 
// `freeaddrinfo` when it's no longer needed.

// <Return Value:
// The function returns 0 on success or an error code (non-zero value) if an error occurs.

// <Example:
// Here's a simple example demonstrating the usage of `getaddrinfo` to resolve a hostname and
// print its associated IP addresses:

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
	const char *hostname = "www.google.com"; // Hostname to resolve
	const char *service = "http"; // Service name or port number

	struct addrinfo hints, *res, *p;
	int status;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET; // Allow IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // Socket type (stream)

	if ((status = getaddrinfo(hostname, service, &hints, &res)) != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return 1;
	}

	// Loop through the linked list of results and print IP addresses
	for (p = res; p != nullptr; p = p->ai_next) {
		void *addr;
		if (p->ai_family == AF_INET) { // IPv4
			struct sockaddr_in *ipv4 = reinterpret_cast<struct sockaddr_in *>(p->ai_addr);
			addr = &(ipv4->sin_addr);
		} else { // IPv6
			struct sockaddr_in6 *ipv6 = reinterpret_cast<struct sockaddr_in6 *>(p->ai_addr);
			addr = &(ipv6->sin6_addr);
		}

		char ipstr[INET6_ADDRSTRLEN];
		inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
		std::cout << "IP Address: " << ipstr << std::endl;
	}

	freeaddrinfo(res); // Free memory allocated for results

	return 0;
}

// This example resolves the hostname "www.example.com" for the service "http" and prints the associated
// IP addresses. It utilizes `getaddrinfo` to perform the resolution and then iterates through the results
// to extract and print the IP addresses.