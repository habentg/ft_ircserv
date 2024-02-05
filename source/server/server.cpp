/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:30:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/05 14:59:48 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

/* ------------------------------------------------------------------------------------------ */
/*                           Constructing our server object                                   */
/* ------------------------------------------------------------------------------------------ */
Server* Server::serverInstance = NULL;

Server::Server(unsigned short int  portNumber, std::string password) : _port_number(portNumber), _passwd(password) {
    std::cout << "Server created!" << std::endl;
}

Server& Server::createServerInstance(double portNumberDouble, std::string password) {
    if (!serverInstance)
        serverInstance = new Server(static_cast<unsigned short int>(portNumberDouble), password);
    return *serverInstance;
}

Server::~Server(void) {
    std::cout << "Server Destructor Called!";
}
/* ------------------------------------------------------------------------------------------ */
/*                           Exception methods                                                */
/* ------------------------------------------------------------------------------------------ */
const char* Server::exc::what() const throw() {
    return "Something went wrong!";
}

/* ------------------------------------------------------------------------------------------ */
/*                           getters and other simple methods                                 */
/* ------------------------------------------------------------------------------------------ */
unsigned short int Server::getServerPortNumber(void) const {
    return this->_port_number;
}


/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */
int Server::server_listen_setup(char *portNumber) {
    int sockfd;
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;
    int status = getaddrinfo(NULL, portNumber, &hints, &res);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Error creating socket");
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
        throw std::runtime_error("Error binding socket");
    freeaddrinfo(res);
    if (listen(sockfd, SOMAXCONN) == -1)
        throw std::runtime_error("Error listening on socket");
    return (sockfd);
}