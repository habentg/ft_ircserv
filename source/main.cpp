/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:44:30 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/05 16:42:20 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

int main(int ac, char **av) {
    // check number of arguments, and validate port number
    double portNumberDouble = validate_input(ac, av);
    if (portNumberDouble == -1)
        return 1;
    // instantiating a single server object and assigning port-number & password 
    Server& serverObj = Server::createServerInstance(portNumberDouble, std::string(av[1]));
    try {
        int sockfd = serverObj.server_listen_setup(av[1]);
        std::cout << "-------- LISTENING FOR CONNECTIONS .... ---------\n";
        while (1) {
            // signal will have to be setup here!
            struct sockaddr_storage client_addr;
            socklen_t client_addr_size = sizeof(client_addr);
            int newFd = accept(sockfd, (sockaddr *)&client_addr, &client_addr_size);
            std::cout << "-------- ACCEPTED A NEW CONNECTION ---------\n";
            if (newFd == -1)
                throw std::runtime_error("Error accepting connections!");
            char ipstr[INET6_ADDRSTRLEN];
            if (client_addr.ss_family == AF_INET) { // IPv4
                const struct sockaddr_in* addr_in = reinterpret_cast<const struct sockaddr_in*>(&client_addr);
                inet_ntop(AF_INET, &(addr_in->sin_addr), ipstr, sizeof(ipstr));
            } else { // IPv6
                const struct sockaddr_in6* addr_in6 = reinterpret_cast<const struct sockaddr_in6*>(&client_addr);
                inet_ntop(AF_INET6, &(addr_in6->sin6_addr), ipstr, sizeof(ipstr));
            }
            std::cout << "->client addr: " << ipstr << std::endl;
            // reviev
            char buffer[1024];
            int buffer_size = sizeof(buffer);

            int bytes_received = recv(newFd, buffer, buffer_size, 0);
            if (bytes_received == -1) {
                perror("recv failed");
                // Handle error
            } else if (bytes_received == 0) {
                throw std::runtime_error("Connection closed");
                // Handle connection closure
            } else {
                buffer[bytes_received] = '\0'; // Null-terminate the received data
                std::cout << "->Message: [" << std::string(buffer).substr() << "] received!\n";
            }
            //send
        }
    } catch (std::string &err) {
        std::cerr << err << std::endl;
    } catch (const char* err) {
        std::cerr << err << std::endl;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "ERROR!!" << std::endl;
    }
    delete &serverObj;
    return 0;
}