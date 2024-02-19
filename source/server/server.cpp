/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:30:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/19 22:25:45 by hatesfam         ###   ########.fr       */
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

std::string Server::getPassword(void) {
    return this->_passwd;
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

void Server::doStuff(int clientFd, std::string msg) {
    std::cout << "registered Client: [" << clientFd << "] has sent: " << msg << std::endl;
}

void    Server::authenticateClient(Client *cl, std::string msg) {
    int space_index = msg.find(' ');
    if (msg.substr(0, space_index) == "PASS") {
        if (msg.substr(space_index + 1) == this->_passwd) {
            cl->setIsAuthenticated(true);
            std::cout << "Client: [" << cl->getClientFd() << "] has authenticated\n";
        }
        else {
            this->sendMsgToClient(cl->getClientFd(), WRONG_PASS(cl->getIpAddr()));
            if (cl->getWrongPassCount() == 3) {
                this->removeClient(cl->getClientFd());
            } else
                cl->setWrongPassCount(cl->getWrongPassCount() + 1);
            return ;
        }
    }
    if (msg.substr(0, space_index) == "NICK") {
        std::map<int, Client *>::iterator m_it = this->_clients.begin();
        for (; m_it != this->_clients.end(); m_it++)
        {
            if (m_it->second->getNICK() == msg.substr(space_index + 1) && m_it->first != cl->getClientFd()) {
                this->sendMsgToClient(cl->getClientFd(), GOODBYE(cl->getIpAddr(), cl->getNICK()));
                this->removeClient(cl->getClientFd());
                return ;
            }
        }
        cl->setNICK(msg.substr(space_index + 1));
        std::cout << "Client: " << cl->getClientFd() << " has set his nickname to [" << cl->getNICK() << "]\n";
    }
    if (msg.substr(0, space_index) == "USER") {
        std::cout << "here\n";
        std::string realname = msg.substr(msg.find(':'));
        cl->setRealName(realname);
        std::vector<std::string> sp = split(msg, 32);
        cl->setHostName(sp[3]);
        cl->setUserName(sp[1]);
    }
}

void Server::userAuthentication(Client* cl, std::string cap, bool isCap) {
    if (isCap) {
        std::vector<std::string> caps = split(cap, 13);
        std::vector<std::string>::iterator it = caps.begin();
        it++;
        for (; it != caps.end(); it++) {
            authenticateClient(cl, (*it));
        }
    } else {
        std::vector<std::string> msg_arr = split(cap, 10);
        authenticateClient(cl, msg_arr[0]);
    }
        // // send welcome message
    if (!cl->getIsregistered() && cl->getIsAuthenticated() && cl->getNICK() != "" && cl->getUserName() != "") {
        cl->setIsregistered(true);
        std::cout << "Client: " << cl->getClientFd() << " has registered\n";
        this->sendMsgToClient(cl->getClientFd(), RPL_WELCOME(cl->getIpAddr(), cl->getHostName(), cl->getUserName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_YOURHOST(cl->getIpAddr(), cl->getHostName()));
        this->sendMsgToClient(cl->getClientFd(), RPL_CREATED(cl->getIpAddr(), cl->getHostName()));
        this->sendMsgToClient(cl->getClientFd(), RPL_MYINFO(cl->getIpAddr(), cl->getHostName()));
        this->sendMsgToClient(cl->getClientFd(), RPL_ISUPPORT(cl->getIpAddr()));
        this->sendMsgToClient(cl->getClientFd(), RPL_MOTDSTART(cl->getIpAddr(), cl->getHostName()));
        this->sendMsgToClient(cl->getClientFd(), RPL_MOTD(cl->getIpAddr()));
        this->sendMsgToClient(cl->getClientFd(), RPL_ENDOFMOTD(cl->getIpAddr(), cl->getHostName()));
    }
    // if (!cl->getIsregistered()) {
    //     std::cout << "Client: " << cl->getClientFd() << " has not registered\n";
    //     std::string goodbyeMessage = ":server.example.com 451 " + cl->getNICK() + " :You have not registered\n";
    //     this->sendMsgToClient(cl->getClientFd(), goodbyeMessage);
    //     this->removeClient(cl->getClientFd());
    // }
}

void Server::registerClient(int clientFd, std::string msg) {
    Client *client = this->_clients[clientFd];
    if (msg.substr(0, 6) == "CAP LS") {
        this->sendMsgToClient(clientFd, CAP_LS_RESP(client->getIpAddr()));
    }
    if (msg.substr(0, 7) == "CAP REQ") {
        this->sendMsgToClient(clientFd, CAP_ACK_RESP(client->getIpAddr()));
    }
    if (msg.substr(0, 7) == "CAP END") {
        this->userAuthentication(client, msg, true);
    }
    else {
        this->userAuthentication(client, msg, false);
    }
}

void Server::removeClient(int clientFd) {
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
}
void Server::recieveMsg(int clientFd) {
    char buffer[1024];
    int buffer_size = sizeof(buffer);

    std::memset(buffer, 0, buffer_size);
    int bytes_received = recv(clientFd, buffer, buffer_size, 0);
    if (bytes_received == -1)
        throw std::runtime_error("Error Receiving message from client");
    else if (bytes_received == 0)
        throw std::runtime_error("Error Connection closed with the client");
    buffer[bytes_received] = '\0'; // Null-terminate the received data
    try
    {
        std::string msg = std::string(buffer);
        if (!this->getClient(clientFd)->getIsregistered())
            this->registerClient(clientFd, msg);
        // else
        //     this->doStuff(clientFd, msg);

        /* NOTE: server is desconnecting after "NO PONG in 301 seconds ... think about it"*/
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}