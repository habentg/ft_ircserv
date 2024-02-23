// The `setsockopt` function in C and C++ is used to set options or
// configurations for a specified socket. It allows you to manipulate
// various socket options at the API level, giving control over the
// behavior of the socket.

// The syntax of the `setsockopt` function is as follows:

// ```cpp
// int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
// ```

// Parameters:
// - `sockfd`: The file descriptor of the socket for which the option is to be set.
// - `level`: Specifies the protocol level at which the option resides.
// Common levels include `SOL_SOCKET` for manipulating options at the socket level
// or specific protocol levels like `IPPROTO_TCP`, `IPPROTO_IP`, etc.
// - `optname`: Indicates the specific option to set for the socket.
// - `optval`: A pointer to the value to be set for the option.
// - `optlen`: The size of the value pointed to by `optval`.

// The `setsockopt` function is versatile and can be used to set various socket
// options depending on the `optname` and `level` parameters. Some common uses include:

// - Configuring socket behavior, such as setting socket timeouts (`SO_RCVTIMEO`, `SO_SNDTIMEO`).
// - Enabling or disabling socket options like reuse of addresses (`SO_REUSEADDR`),
// 	enabling broadcast (`SO_BROADCAST`), etc.
// - Setting options related to packet handling, such as the maximum segment size
// 	(`TCP_MAXSEG` for TCP sockets), type of service (`IP_TOS`), etc.

// Here's a simple example demonstrating how `setsockopt` might be used to set the
// `SO_REUSEADDR` option:


#include <iostream>
#include <sys/socket.h> // AF_INET   SOCK_STREAM    SOL_SOCKET   SO_REUSEADDR
#include <unistd.h>

int main() {
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cerr << "Error creating socket\n";
		return 1;
	}
	// Enable address reuse option for the socket
	int optval = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		std::cerr << "Error setting socket option\n";
		return 1;
	}
	// Further socket operations can be performed

	// Close the socket when done
	close(sockfd);

	return 0;
}

// Always ensure that the parameters passed to `setsockopt` are correct and
// appropriate for the specific socket and desired behavior, as incorrect usage
// can lead to unexpected behavior or errors in the functioning of the socket.