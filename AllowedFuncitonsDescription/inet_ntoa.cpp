/* The `inet_ntoa` function is used to convert a 32-bit network
byte order binary representation of an IPv4 address back into
its standard dotted-decimal notation.

Here's an example of how you might use `inet_ntoa`: */

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main() {
    struct in_addr addr;
    addr.s_addr = inet_addr("192.168.0.1");

    // Convert network address to a dotted-decimal notation
    char *ip_address = inet_ntoa(addr);

    if (ip_address != NULL) {
        printf("Network address: %u, IP address: %s\n", addr.s_addr, ip_address);
    } else {
        printf("Conversion failed\n");
    }

    return 0;
}

/* This function is considered somewhat outdated due to its limitations
and potential thread-unsafety. In modern code, `inet_ntop` is preferred,
as it's more secure and supports both IPv4 and IPv6 address conversions. */