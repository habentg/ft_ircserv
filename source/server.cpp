/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:30:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/24 02:12:51 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

/* ------------------------------------------------------------------------------------------ */
/*                           Constructing our server object                                   */
/* ------------------------------------------------------------------------------------------ */
Server* Server::serverInstance = NULL;

Server::Server(unsigned short int  portNumber, std::string password) : _serverPort(portNumber) {
    this->_serverSocketFd = 0;
    this->_serverPassword = password;
    this->_serverHostName = "";
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
    return this->_serverPort;
}

std::vector<struct pollfd>& Server::getFdArray(void) {
    return this->_fdsArray;
}

Client* Server::getClient(int clientFd) {
    return this->_clients[clientFd];
}

std::string Server::getPassword(void) const {
    return this->_serverPassword;
}
std::string Server::getServerHostName(void) const {
    return this->_serverHostName;
}

size_t    Server::getNumberOfClients(void) const {
    return this->_clients.size();
}

/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */
void Server::server_listen_setup(char *portNumber) {
    int sockfd;
    int enableer = 1; // for setsockopt() -> to avoid "is already in use" error
    struct addrinfo hints;

    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) != 0)
        throw std::runtime_error("Error: Hostname resolution failed");
    this->_serverHostName = std::string(hostname);
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    struct addrinfo *res;
    int status = getaddrinfo(NULL, portNumber, &hints, &res);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    if (res == NULL)
        throw std::runtime_error("Error: Address resolution result is NULL");
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Error: creating socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enableer, sizeof(int)))
        throw std::runtime_error("Error: setsockopt");
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < -1)
        throw std::runtime_error("Error: fcntl");
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
        throw std::runtime_error("Error: binding socket");
    freeaddrinfo(res);
    if (listen(sockfd, SOMAXCONN) == -1)
        throw std::runtime_error("Error: listening on socket");
    this->_serverSocketFd = sockfd;
    this->addToFdArray(sockfd); // add our listner socket fd to the array so we can watch for new connection request on it.
    
    std::cout << "-------- Listening on " << this->_serverHostName << ":" << this->_serverPort<< ".... ---------\n";
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

int    Server::isClientAvailable(int clientFd, std::string nick) const {
    std::map<int, Client *>::const_iterator map_it = this->_clients.begin();
    for (; map_it != this->_clients.end(); map_it++)
    {
        // if (map_it->first != clientFd && lowerCaseString(map_it->second->getNICK()) == lowerCaseString(nick)) {
        if (map_it->first != clientFd && (map_it->second->getNICK()) == (nick)) {
            std::cout << "{NICK: " << map_it->second->getNICK() << " & " << nick << " are the same/close}\n";
            return (map_it->first);
        }
    }
    return 0;
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
    int newClientFd = accept(this->_serverSocketFd, (sockaddr *)&client_addr, &client_addr_size);
    if (newClientFd == -1) {
        throw std::runtime_error("Error: accepting connections!");
    }
    this->addToFdArray(newClientFd);
    struct sockaddr *clientInfo = (struct sockaddr *)&client_addr;
    std::cout << "New Client accepted: " << inet_ntoa(((struct sockaddr_in *)clientInfo)->sin_addr)
        << ":" << ntohs(((struct sockaddr_in *)clientInfo)->sin_port) << " at FD: " << newClientFd <<std::endl;
    saveClientINfo(newClientFd, clientInfo);
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
        throw std::runtime_error("Error: sending message to client");
}

/* After connection establishment:
    1. client is sending a PING and we should respond with a PONG (or it will exit after 301 seconds)
    2. PRIVMSG:-
        * [PRIVMSG <recipient nickname> :<message to be sent>] - is the format
        * we should check if nick
*/
std::string constructReplayMsg(std::string rpl) {
    std::string str = rpl + "\r\n";
    return str;
}
void Server::doStuff(Client* client, Command *command) {
    if (command->cmd == "PING") // somehow this shit is not working, I am sending PONG replay the same way a real server is doing it ... FUCK
        this->sendMsgToClient(client->getClientFd(), PONG(this->getServerHostName()));
    if (command->cmd == "PRIVMSG") {
        std::cout << "is we here\n";
        // if (isChannelName(command->params[0]) == true)
        //     sendToChannel();
        if (validNickName(command->params, client->getClientFd(), this, command->cmd) == false) {
            return ;
        }
        std::cout << "is we here --- valid\n";
        int recieverFd = this->isClientAvailable(client->getClientFd(), command->params[0]);
        if (recieverFd == 0) {
            std::cout << "No such client\n";
            return ;
        }
        std::cout << "is we here --- to be sent\n";
        this->sendMsgToClient(recieverFd, constructReplayMsg(command->params[1]));
        return ;

    }
    if (command->cmd == "JOIN") {
        std::cout << "CHANNELL JOIN\n";
        /* creating/joining a channel:
            -> need a vector in the server to hold all the channel names,
            -> each channel will have:
                * an chanOp - the one who created it (can give to others as well)
                * a list of all users -
                * number of users limit
        */
    }
    std::cout << "registered Client: [" << client->getClientFd() << "] has sent: " << command->cmd << std::endl;
}

bool    Server::authenticateClient(Client *cl, Command *command) {
    if (command->cmd == "PASS") {
        command->password(cl, this);
    }
    else if (cl->getIsAuthenticated()) {
        if (command->cmd == "NICK")
            return (command->nickname(cl, this));
        if (command->cmd == "USER")
            command->user(cl, this);
    }
    else {
        // we will check if sending "password needed" is allowd!
        std::cout << "["<< command->cmd <<"]: You have to be authenitcated to continue ...\n";
    }
    return true;
}

void Server::userAuthenticationAndWelcome(Client* cl, Command *command) {
    // authenticate
    if (authenticateClient(cl, command) == false)
        return ;
        // // send welcome message
    if (cl->getIsAuthenticated() && cl->getUserName() != "") {
        cl->setIsregistered(true);
        this->sendMsgToClient(cl->getClientFd(), RPL_WELCOME(this->getServerHostName(), cl->getUserName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_YOURHOST(this->getServerHostName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_CREATED(this->getServerHostName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_MYINFO(this->getServerHostName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_ISUPPORT(this->getServerHostName(), cl->getNICK()));
        /* MOTD */
        this->sendMsgToClient(cl->getClientFd(), RPL_MOTDSTART(this->getServerHostName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_MOTD(this->getServerHostName(), cl->getNICK()));
        this->sendMsgToClient(cl->getClientFd(), RPL_ENDOFMOTD(this->getServerHostName(), cl->getNICK()));
        std::cout << "Client: " << cl->getClientFd() << " is Connected!!\n";
    }
}

void Server::registerClient(Client *client, Command *command) {

    if (command->cmd == "CAP") {
        if (command->params[0] == "LS")
            (this->sendMsgToClient(client->getClientFd(), CAP_LS_RESP(this->getServerHostName())));
        else if (command->params[0] == "REQ")
            this->sendMsgToClient(client->getClientFd(), CAP_ACK_RESP(this->getServerHostName()));
        else if (command->params[0] == "END")
            std::cout<<"CAP End response sent\n";
    }
    else {
        this->userAuthenticationAndWelcome(client, command);
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
        throw std::runtime_error("Error: Receiving message from client");
    else if (bytes_received == 0)
        return ;
    buffer[bytes_received] = '\0'; // Null-terminate the received data
    try
    {
        /* command construction */
        /* Only parse and process a message once you encounter the \r\n at the end of it.
            If you encounter an empty message, silently ignore it.
        */
        std::string msg = std::string(buffer);
        std::vector<std::string> arr_of_cmds = split(msg, '\0');
        std::vector<std::string>::iterator it = arr_of_cmds.begin();
        for (; it != arr_of_cmds.end(); ++it) {
            Command *cmd = new Command((*it));
            Client *client = this->_clients[clientFd];
            // std::cout << "MSG: [" << msg << "] CMD: " << cmd->cmd << " has: " << cmd->params.size() << " params\n";
            if (client->IsClientConnected() == false) {
                this->registerClient(client, cmd);
            }
            else
                this->doStuff(client, cmd);
            delete cmd; // we dont need the command anymore
        }

        /* NOTE: server is desconnecting after "NO PONG in 301 seconds ... think about it"*/
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}