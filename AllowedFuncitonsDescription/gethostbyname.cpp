// The `gethostbyname` function, which is part of the C Standard Library `<netdb.h>`,
// is used to retrieve host information by providing a hostname. However, `gethostbyname`
// is considered deprecated due to its limitations and security concerns. Instead,
// the more modern and recommended function is `getaddrinfo`.

// Nonetheless, `gethostbyname` was historically used to retrieve host information
// based on a hostname.

// Here's an explanation of how `gethostbyname` works:

// Syntax:
// ```c
// struct hostent *gethostbyname(const char *name);
// ```

// - `name`: A pointer to a null-terminated string containing the hostname for
// which you want to retrieve information.

// <Return Value:
// The function returns a pointer to a `struct hostent` on success or `NULL` on failure.

// <struct hostent:
// The `struct hostent` contains information about a host, including its name,
// list of IP addresses associated with the hostname, address type, and more.

// Example (using `gethostbyname`):

#include <iostream>
#include <arpa/inet.h> // inet_ntoa
#include <netdb.h>

int main() {
	const char *hostname = "www.axample.com"; // Hostname to retrieve information about

	struct hostent *host_info = gethostbyname(hostname);
	if (host_info == nullptr) {
		perror("gethostbyname");
		return 1;
	}

	std::cout << "Official Host Name: " << host_info->h_name << std::endl;

	// Print all IP addresses associated with the hostname
	char **addr = host_info->h_addr_list;
	while (*addr != nullptr) {
		struct in_addr *address = reinterpret_cast<struct in_addr *>(*addr);
		std::cout << "IP Address: " << inet_ntoa(*address) << std::endl;
		++addr;
	}

	return 0;
}

// This example demonstrates how `gethostbyname` can be used to retrieve host
// information, such as the official hostname and its associated IP addresses.
// However, note that `gethostbyname` is deprecated and has limitations, such
// as not supporting IPv6.

// For modern applications, consider using `getaddrinfo`, which provides more
// functionality, support for IPv6, and better error handling capabilities.