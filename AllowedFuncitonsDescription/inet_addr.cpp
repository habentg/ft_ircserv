/* The `inet_addr` function is used to convert an IPv4 address in
its standard dotted-decimal notation (like "192.168.0.1") into
a 32-bit network byte order binary representation suitable for
use in network functions and structures.

Here's an example of its usage: */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main() {
	const char *ip_address = "192.168.0.1";

	// Convert IP address to a 32-bit network byte order
	unsigned long network_address = inet_addr(ip_address);

	if (network_address != INADDR_NONE) {
		printf("IP address: %s, Network address: %lu\n", ip_address, network_address);
	} else {
		printf("Invalid IP address\n");
	}

	return 0;
}

/* Please note that `inet_addr` returns `INADDR_NONE` (usually `-1`)
if the input string is not a valid IPv4 address. Additionally, `inet_addr`
is considered a legacy function and might not be available or recommended
in all modern systems or situations. In modern code, you might prefer to
use `inet_pton` for IPv4 and IPv6 address conversion, as it provides more 
robust error handling. */