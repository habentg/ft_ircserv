/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:30:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/13 10:15:59 by hatesfam         ###   ########.fr       */
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
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); it++)
        delete it->second;
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

Client* Server::getClient(int clientFd) {
    return this->_clients[clientFd];
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
    // also initialize all of them for conditional jumps avoidance
    struct pollfd newConFdStruct;
    newConFdStruct.fd = newfd;
    newConFdStruct.events = POLLIN; // event to watch for is POLLIN
    newConFdStruct.revents = 0; // revents is set to 0
    this->getFdArray().push_back(newConFdStruct);
}

/* ------------------------------------------------------------------------------------------ */
/*                           creating a client for every fd                                   */
/* ------------------------------------------------------------------------------------------ */

void Server::saveClientINfo(int clientFd, struct sockaddr *clientInfo) {
    Client *client = new Client(clientFd, clientInfo);
    this->_clients.insert(std::pair<int, Client *>(clientFd, client));
    // std::cout << "Client info saved! : " << this->_clients.size() << "\n";
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

void Server::sendMsgToAllClients(std::string msg) {
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); it++)
        this->sendMsgToClient(it->first, msg);
}
void Server::sendMsgToClient(int clientFd, std::string msg) {
    int bytes_sent = send(clientFd, msg.c_str(), msg.length(), 0);
    if (bytes_sent == -1)
        throw std::runtime_error("Error sending message to client");
}

void Server::registerClient(int clientFd, std::string msg) {
    Client *client = this->_clients[clientFd];
    if (msg.substr(0, 4) == "view") {
        this->sendMsgToClient(clientFd, "------------HERE IS YOUR CLIENT INFO--------\n");
        std::string mymsg = "NICK: [" + client->getNICK() + "]\nUSERNAME: [" 
            + client->getUserName() + "]\nIsAuthenticated: [" + std::to_string(client->getIsAuthenticated()) + 
                "]\nIsRegistered: [" + std::to_string(client->getIsregistered()) + "]\n";
        this->sendMsgToClient(clientFd, mymsg);
        this->sendMsgToClient(clientFd, "------------########################--------\n");
        std::cout << "Client: " << clientFd << " has viewed its info!\n";
        return ;
    }
    /* ALL THE COMMMANDS HERE!!!!! */
    /* FROM SUBJECT:◦ You must be able to authenticate, set a nickname, a username, join a channel,
                      send and receive private messages using your reference client. */
    if (!client->getIsAuthenticated()) { // validate PASS cmd
        if (msg.substr(0, 4) == "PASS") {
            if (msg[5] == '\0') {
                this->sendMsgToClient(clientFd, "ERROR : enter password {PASS <server password>}\n");
                return ;
            }
            if (msg.substr(5) == this->_passwd) {
                client->setIsAuthenticated(true);
                std::cout << "Client: " << clientFd << " has been authenticated\n";
            } else {
                this->sendMsgToClient(clientFd, "ERROR :Access denied! correct password required\n");
                std::cout << "Client: " << clientFd << " has failed to authenticate\n";
            }
        } else {
            std::cout << "Client: " << clientFd << " has failed to authenticate\n";
            this->sendMsgToClient(clientFd, "ERROR : password required for authentication\n");
        }
    }
    else {    // bunch of ifs for all the commands
        if (msg.substr(0, 4) == "PASS" && client->getIsAuthenticated()) { // if client is already authenticated
            std::cout << "Client: " << clientFd << " has Already been authenticated\n";
        }
        else if (msg.substr(0, 4) == "NICK") {
            std::map<int, Client *>::iterator it = this->_clients.begin();
            for (; it != this->_clients.end(); it++)
            {
                if (it->second->getNICK() == msg.substr(5) && it->first != clientFd) {
                    std::cout << "Client: " << clientFd << " entered already used up nickname\n";
                    this->sendMsgToClient(clientFd, "ERROR : Nickname already in use\n");
                    break ;
                }
            }
            if (it == this->_clients.end()) {
                client->setNICK(msg.substr(5));
                std::cout << "Client: " << clientFd << " has set his nickname to [" << client->getNICK() << "]\n";
            }
        }
        else if (msg.substr(0, 8) == "USERNAME") {
            client->setUserName(msg.substr(9));
            std::cout << "Client: " << clientFd << " has set his username to [" << client->getUserName() << "]\n";
        }
    }
    // send welcome message
    if (!client->getIsregistered() && client->getIsAuthenticated() && client->getNICK() != "" && client->getUserName() != "") {
        client->setIsregistered(true);
        std::string welcomeMsg = "Welcome to the server " + client->getNICK() + "! be a good boy ...\n";
        this->sendMsgToClient(clientFd, welcomeMsg);
    }
}

void Server::recieveMsg(int clientFd) {
    char buffer[1024];
    int buffer_size = sizeof(buffer);
    
    int bytes_received = recv(clientFd, buffer, buffer_size, 0);
    if (bytes_received == -1)
        throw std::runtime_error("Error Receiving message from client");
    else if (bytes_received == 0)
        throw std::runtime_error("Error Connection closed with the client");
    buffer[bytes_received] = '\0'; // Null-terminate the received data
    try
    {
        std::string msg = std::string(buffer, (bytes_received - 1));
        if (msg == "QUIT") { // if the client sends QUIT, we disconnect him
            std::cout << "Client: " << clientFd << " has disconnected\n";
            delete this->getClient(clientFd); // delete the client object
            this->_clients.erase(clientFd); // remove the client from the map
            // remove the fd STRUCT from the array as weelllllllll
            std::vector<struct pollfd>::iterator it = this->_fdsArray.begin();
            for (; it != this->_fdsArray.end(); it++) {
                if ((*it).fd == clientFd) {
                    this->_fdsArray.erase(it);
                    break ;
                }
            }
            close(clientFd);
            return ;
        }
        if (!this->getClient(clientFd)->getIsregistered())
            this->registerClient(clientFd, msg);
        // else
        //     this->doStuff(clientFd, msg); --> for later
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}