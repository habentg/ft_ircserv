/* `sigaction` is a more modern and sophisticated way to handle signals
compared to the older `signal` function in C and C++.

It allows for finer control over signal handling and provides a more
robust interface. Some of the advantages of `sigaction` over `signal` include:

- **More Control:** `sigaction` provides more precise control over how signals
are handled, including options to block signals during their handler's execution.
- **Portable:** The behavior of `signal` varies across different systems, whereas
`sigaction` provides a more consistent behavior across platforms.
- **Reentrancy:** It's more reentrant and safer to use in multithreaded environments.
- **Handling Actions:** It allows specifying additional actions and flags for signal
handling.

Here's an example of using `sigaction`: */

#include <stdio.h>
#include <signal.h>

void sigint_handler(int signum) {
	printf("Received SIGINT (%d)\n", signum);
	// Add your custom handling logic here
}

int main() {
	struct sigaction sa;
	sa.sa_handler = sigint_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL); // Registering the handler for SIGINT

	// Your main program loop
	while (1) {
		// Do some work here
	}

	return 0;
}

/* In this example, `sigaction` is used to set up the signal handler for `SIGINT`
in a manner similar to `signal`. The `sa_handler` field in the `struct sigaction`
specifies the function to be called when the signal is received. Other fields in
the `struct sigaction` allow for more advanced signal handling configurations. */