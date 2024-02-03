// Functions like `htons`, `htonl`, `ntohs`, and `ntohl` are used to convert
// between host byte order and network byte order (big-endian or little-endian).

// - `htons` (Host to Network Short): Converts a 16-bit quantity from host byte
// 	order to network byte order.

// - `htonl` (Host to Network Long): Converts a 32-bit quantity from host byte
// 	order to network byte order.

// - `ntohs` (Network to Host Short): Converts a 16-bit quantity from network
// 	byte order to host byte order.

// - `ntohl` (Network to Host Long): Converts a 32-bit quantity from network
// 	byte order to host byte order.

// These functions are typically used when working with networking code to ensure
// data is sent and received in the correct byte order, especially when communicating
// between different systems that might use different byte orders.

// Here's a simple example:

#include <stdio.h>
#include <arpa/inet.h>

int main() {
	unsigned short host_short = 1234;
	unsigned long host_long = 567890;

	// Convert from host to network byte order
	unsigned short network_short = htons(host_short);
	unsigned long network_long = htonl(host_long);

	printf("Host short: %hu, Network short: %hu\n", host_short, network_short);
	printf("Host long: %lu, Network long: %lu\n", host_long, network_long);

	// Convert from network to host byte order
	unsigned short converted_short = ntohs(network_short);
	unsigned long converted_long = ntohl(network_long);

	printf("Network short: %hu, Converted short: %hu\n", network_short, converted_short);
	printf("Network long: %lu, Converted long: %lu\n", network_long, converted_long);

	return 0;
}

// These functions are especially important when dealing with protocols that have
// specified byte order requirements for communication, like TCP/IP, where the network
// byte order is big-endian.