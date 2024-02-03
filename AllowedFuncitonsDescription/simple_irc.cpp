// #include <iostream>
// #include <vector>
// #include <sys/types.h>
// #include <sys/event.h>
// #include <sys/time.h>
// #include <unistd.h>
// #include <fcntl.h>
// #include <netinet/in.h>
// #include <arpa/inet.h>
// #include <sys/socket.h>

// class User {
// public:
//     explicit User(int socket_fd) : socket_fd(socket_fd) {}

//     int getSocketFD() const {
//         return socket_fd;
//     }

// private:
//     int socket_fd;
// };

// int main() {
//     const int PORT = 8080;
//     const int MAX_EVENTS = 10;
//     const int MAX_CLIENTS = 10;

//     int server_fd, kq;
//     std::vector<User> clients;

//     struct sockaddr_in server_addr;
//     struct kevent event, events[MAX_EVENTS];

    
//     if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
//         perror("Socket creation failed");
//         return -1;
//     }

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     // Bind the socket
//     if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
//         perror("Bind failed");
//         return -1;
//     }

//     // Listen
//     if (listen(server_fd, MAX_CLIENTS) < 0) {
//         perror("Listen failed");
//         return -1;
//     }

//     // Create kqueue instance
//     if ((kq = kqueue()) == -1) {
//         perror("Kqueue creation failed");
//         return -1;
//     }

//     EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

//     // Add server socket to kqueue
//     if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
//         perror("Kqueue control failed");
//         return -1;
//     }
//   std::cout << "Server started. Listening on port " << PORT << std::endl;

//     while (true) {
//         int num_events = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
//         if (num_events == -1) {
//             perror("Kqueue wait failed");
//             return -1;
//         }

//         for (int i = 0; i < num_events; ++i) {
//             if (events[i].ident == server_fd) {
//                 // Accept incoming connection
//                 int client_socket = accept(server_fd, nullptr, nullptr);
//                 if (client_socket == -1) {
//                     perror("Accept failed");
//                     continue;
//                 }

//                 // Make socket non-blocking
//                 fcntl(client_socket, F_SETFL, fcntl(client_socket, F_GETFL, 0) | O_NONBLOCK);

//                 // Add client socket to kqueue
//                 EV_SET(&event, client_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
//                 User newUser(client_socket);
//                 clients.push_back(newUser);

//                 if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
//                     perror("Kqueue control failed");
//                     return -1;
//                 }

//                 std::cout << "New client connected. FD: " << client_socket << std::endl;
//             } else {
//                 // Handle client data
//                 char buffer[1024];
//                 int recv_result = recv(events[i].ident, buffer, sizeof(buffer), 0);
//                 if (recv_result <= 0) {
//                     // Connection closed or error, remove client from kqueue (same as previous code)

//                     std::cout << "Client FD " << events[i].ident << " disconnected." << std::endl;
//                 } else {
//                     // Relay received data to all other clients
//                     for (const User &client : clients) {
//                         if (client.getSocketFD() != events[i].ident && client.getSocketFD() != server_fd) {
//                             send(client.getSocketFD(), buffer, recv_result, 0);
//                         }
//                     }
//                 }
//             }
//         }
//     }

//     return 0;
// }

#include <iostream>
#include <vector>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>

class User {
public:
	User(int socket_fd) : socket_fd(socket_fd) {}

	int getSocketFD() const {
		return socket_fd;
	}

private:
	int socket_fd;
};

int main() {
	const int PORT = 8080;
	const int MAX_EVENTS = 10;
	const int MAX_CLIENTS = 10;

	int server_fd, kq;
	std::vector<User> clients;

	struct sockaddr_in server_addr;
	struct kevent event, events[MAX_EVENTS];

	// Create socket
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("Socket creation failed");
		return -1;
	}

	// Set server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);

	// Bind the socket
	if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		return -1;
	}

	// Listen
	if (listen(server_fd, MAX_CLIENTS) < 0) {
		perror("Listen failed");
		return -1;
	}

	// Create kqueue instance
	if ((kq = kqueue()) == -1) {
		perror("Kqueue creation failed");
		return -1;
	}

	// Add server socket to kqueue
	EV_SET(&event, server_fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
		perror("Kqueue control failed");
		return -1;
	}

	std::cout << "Server started. Listening on port " << PORT << std::endl;

	while (true) {
		int num_events = kevent(kq, NULL, 0, events, MAX_EVENTS, NULL);
		if (num_events == -1) {
			perror("Kqueue wait failed");
			return -1;
		}

		for (int i = 0; i < num_events; ++i) {
			if (events[i].ident == server_fd) {
				// Accept incoming connection
				int client_socket = accept(server_fd, NULL, NULL);
				if (client_socket == -1) {
					perror("Accept failed");
					continue;
				}

				// Make socket non-blocking
				fcntl(client_socket, F_SETFL, fcntl(client_socket, F_GETFL, 0) | O_NONBLOCK);

				// Add client socket to kqueue
				EV_SET(&event, client_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
				User newUser(client_socket);
				clients.push_back(newUser);

				if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
					perror("Kqueue control failed");
					return -1;
				}

				std::cout << "New client connected. FD: " << client_socket << std::endl;
			} else {
				// Handle client data
				char buffer[1024];
				int recv_result = recv(events[i].ident, buffer, sizeof(buffer), 0);
				if (recv_result <= 0) {
					// Connection closed
						// close(events[i].data.fd);
						// epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
					std::cout << "Client FD " << events[i].ident << " disconnected." << std::endl;
				} else {
					// Relay received data to all other clients
					for (size_t j = 0; j < clients.size(); ++j) {
						if (clients[j].getSocketFD() != events[i].ident && clients[j].getSocketFD() != server_fd) {
							send(clients[j].getSocketFD(), buffer, recv_result, 0);
						}
					}
				}
			}
		}
	}

	return 0;
}
