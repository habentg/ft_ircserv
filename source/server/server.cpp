/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:30:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/12 10:02:57 by hatesfam         ###   ########.fr       */
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
void Server::server_listen_setup(char *portNumber) {
    int sockfd;
    int enableer = 1; // for setsockopt() -> to avoid "is already in use" error
    struct addrinfo hints;

    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
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
    this->_sockfd = sockfd;
    this->addToFdArray(sockfd); // add our listner socket fd to the array so we can watch for new connection request on it.
    std::cout << "-------- LISTENING FOR CONNECTIONS .... ---------\n";
}


/* ------------------------------------------------------------------------------------------ */
/*                           Adding new fd to the array of fds                                */
/* ------------------------------------------------------------------------------------------ */
void    Server::addToFdArray(int newfd) {
    // validate the new fd first --- comeback later
    struct pollfd newConFdStruct;
    newConFdStruct.fd = newfd;
    newConFdStruct.events = POLLIN; // event to watch for is POLLIN
    this->getFdArray().push_back(newConFdStruct);
}

/* ------------------------------------------------------------------------------------------ */
/*                           creating a client for every fd                                   */
/* ------------------------------------------------------------------------------------------ */

void Server::saveClientINfo(int clientFd, struct sockaddr *clientInfo) {
    this->_clients.insert(std::pair<int, Client *>(clientFd, new Client(clientFd, clientInfo)));
    std::cout << "Client info saved! : " << this->_clients.size() << "\n";
}

/* ------------------------------------------------------------------------------------------ */
/*                           accepting new connection                                         */
/* ------------------------------------------------------------------------------------------ */
void Server::acceptNewConnection(void) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    int newFd = accept(this->_sockfd, (sockaddr *)&client_addr, &client_addr_size);
    if (newFd == -1) {
        throw std::runtime_error("Error accepting connections!");
    }
    this->addToFdArray(newFd);
    struct sockaddr *clientInfo = (struct sockaddr *)&client_addr;
    std::cout << "New Client accepted: " << inet_ntoa(((struct sockaddr_in *)clientInfo)->sin_addr)
        << ":" << ntohs(((struct sockaddr_in *)clientInfo)->sin_port) << std::endl;
    saveClientINfo(newFd, clientInfo);
}

/* ------------------------------------------------------------------------------------------ */
/*                 Recive a message from an existing connection                               */
/* ------------------------------------------------------------------------------------------ */

void Server::validate_message(int clientFd, std::string msg) {
    std::cout << "Validating message: [" << msg << "] from client: " << clientFd << "\n";
    Client *client = this->_clients[clientFd];
    (void)client;
    /* ALL THE COMMMANDS HERE!!!!! */
    // if (!client->getIsAuthenticated()) { // validate PASS cmd
    //     if (msg.)
    // } 
    // else {    // bunch of ifs for all the commands
    //     if (msg == "QUIT") {
    //         std::cout << "Client: " << clientFd << " has disconnected\n";
    //         this->_clients.erase(clientFd);
    //         close(clientFd);
    //     }
    // }
}

void Server::recieveMsg(int clientFd) {
    char buffer[1024];
    int buffer_size = sizeof(buffer);
    
    int bytes_received = recv(clientFd, buffer, buffer_size, 0);
    if (bytes_received == -1)
        throw std::runtime_error("Error Receiving message from client");
    else if (bytes_received == 0)
        throw std::runtime_error("Error Connection closed with the client");
    else {
        buffer[bytes_received] = '\0'; // Null-terminate the received data
        // std::cout << "->Message: [" << std::string(buffer, (bytes_received - 1)) << "] received from Client fd: " << clientFd << "\n";
        this->validate_message(clientFd, std::string(buffer, (bytes_received - 1)));
    }
}