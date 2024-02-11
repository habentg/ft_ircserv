/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:30:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/11 03:54:42 by hatesfam         ###   ########.fr       */
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

std::vector<struct pollfd>& Server::getFdArray(void) {
    return this->_fdsArray;
}
/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */
int Server::server_listen_setup(char *portNumber) {
    int sockfd;
    int enableer = 1; // for setsockopt() -> to avoid "is already in use" error
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;
    int status = getaddrinfo(NULL, portNumber, &hints, &res);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    if (res == NULL)
        throw std::runtime_error("Error Address resolution result is NULL");
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Error creating socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enableer, sizeof(int)))
        throw std::runtime_error("Error setsockopt");
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < -1)
        throw std::runtime_error("Error fcntl");
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
        throw std::runtime_error("Error binding socket");
    freeaddrinfo(res);
    if (listen(sockfd, SOMAXCONN) == -1)
        throw std::runtime_error("Error listening on socket");
    return (sockfd);
}


/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */
void    Server::addToFdArray(int newfd) {
    // validate the new fd first --- comeback later
    struct pollfd newConFdStruct;
    newConFdStruct.fd = newfd;
    newConFdStruct.events = POLLIN;
    this->getFdArray().push_back(newConFdStruct);
}

/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */

void    Server::acceptNewConnection(void) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int newFd = accept(this->getFdArray()[0].fd, (sockaddr *)&client_addr, &client_addr_size);
    std::cout << "new fd: " << newFd << std::endl;
    std::cout << "-------- ACCEPTED A NEW CONNECTION ---------\n";
    if (newFd == -1)
        throw std::runtime_error("Error accepting connections!");
    this->addToFdArray(newFd);
}

void Server::recieveMsg(int clientFd) {
    char buffer[1024];
    int buffer_size = sizeof(buffer);
    
    int bytes_received = recv(clientFd, buffer, buffer_size, 0);
    if (bytes_received == -1) {
        throw std::runtime_error("Error Receiving message from client");
        // Handle error
    } else if (bytes_received == 0) {
        throw std::runtime_error("Error Connection closed with the client");
        // Handle connection closure
    } else {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        std::cout << "->Message: [" << std::string(buffer, (bytes_received - 1)) << "] received from Client fd: " << clientFd << "\n";
    }
    // std::vector
    // // new user
    //     // -> create a new user/client object
    //     // -> add it to the map (its key is its connection fd)
    // Client  *newClient = new Client();
    // this->_clients.insert(std::make_pair(newFd, newClient));    
    // now that we have created the client and we have it added into our map,
        // -> we will search by key, to get the user
}