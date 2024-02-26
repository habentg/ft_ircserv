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
    std::map<std::string, Channel *>::iterator ch_it = this->_channels.begin();
    for (; ch_it != this->_channels.end(); ch_it++)
        delete ch_it->second;
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

Client*   Server::getClientByNick(std::string nick) {
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for(; it != this->_clients.begin(); ++it) {
        if ((*it).second->getNickName() == nick)
            return ((*it).second);
    }
    return (NULL);
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
        // if (map_it->first != clientFd && lowerCaseString(map_it->second->getNickName()) == lowerCaseString(nick)) {
        if (map_it->first != clientFd && (map_it->second->getNickName()) == (nick)) {
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
std::string Server::constructReplayMsg(Client *senderClient, Command *cmd, std::string recieverNick) {
    std::string msg = cmd->raw_cmd.substr(cmd->raw_cmd.find(':') + 1);
    std::string rply = PRIVMSG_RPLY(senderClient->getNickName(), senderClient->getUserName(), this->getServerHostName(), recieverNick, msg);
    return rply;
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
        this->sendMsgToClient(cl->getFd(), ERR_REGISTER_FIRST(this->getServerHostName()));
        return false;
    }
    return true;
}

void Server::userAuthenticationAndWelcome(Client* client, Command *command) {
    // authenticate
    if (authenticateClient(client, command) == false)
        return ;
        // // send welcome message
    if (client->getIsAuthenticated() && client->getUserName() != "" && client->getNickName() != "") {
        client->setIsregistered(true);
        this->sendMsgToClient(client->getFd(), RPL_WELCOME(this->getServerHostName(), client->getUserName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_YOURHOST(this->getServerHostName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_CREATED(this->getServerHostName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_MYINFO(this->getServerHostName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_ISUPPORT(this->getServerHostName(), client->getNickName()));
        /* MOTD */
        this->sendMsgToClient(client->getFd(), RPL_MOTDSTART(this->getServerHostName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_MOTD(this->getServerHostName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_ENDOFMOTD(this->getServerHostName(), client->getNickName()));
        // if there is no serverOp, lets give him the honor
        if (this->_serverOperators.size() == 0)
            this->_serverOperators.insert("@%" + client->getNickName());
        std::cout << "//////server Operator is: [" << *(this->_serverOperators.begin()) << "]//////\n";
        std::cout << "Client: " << client->getFd() << " is Connected!!\n";
    }
}

void Server::registerClient(Client *client, Command *command) {

    if (command->cmd == "CAP") {
        if (command->params[0] == "LS")
            this->sendMsgToClient(client->getFd(), CAP_LS_RESP(this->getServerHostName()));
        else if (command->params[0] == "REQ")
            this->sendMsgToClient(client->getFd(), CAP_ACK_RESP(this->getServerHostName()));
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

/* ================================================================================================================== */
/*                                      Channel related Operations                                                    */
/* ================================================================================================================== */

void    Server::createChannel(std::string chanName, std::string key, Client *creator) {
    Channel* newChan = new Channel(chanName, key, creator);
    this->_channels.insert(std::make_pair(chanName, newChan));
}

bool      Server::doesChanExist(std::string chanName) {
    std::map<std::string, Channel *>::iterator it = this->_channels.lower_bound(chanName);
    if ((*it).first != chanName)
        return false;
    return (true);
}
Channel   *Server::getChanByName(std::string chanName) {
    std::map<std::string, Channel *>::iterator it = this->_channels.lower_bound(chanName);
    return ((*it).second);
}

/* ================================================================================================================== */
/*                                                  DO STUFF                                                          */
/* ================================================================================================================== */
void Server::doStuff(Client* client, Command *command) {
    if (command->cmd == "PING")
        this->sendMsgToClient(client->getFd(), PONG(this->getServerHostName()));
    std::cout << "Full-msg: {" << command->raw_cmd << "}\n";
    if (command->cmd == "PRIVMSG") {
        command->privmsg(client, this);
    }
    /* Channel and channel related features --- big job */
    if (command->cmd == "JOIN") {
        std::cout << "CHANNELL JOIN\n";
        command->join(client, this);
        std::cout << "Channel joined\n";
    }
    /* Not mandatory Commands */
    if (command->cmd == "WHOIS") {
        Client *whoClient = this->getClientByNick(command->params[0]);
        if (whoClient == NULL) {
            std::cout << "+++++++++++++++++++++++did we return\n";
            return ;
        }
        // [euroserv.fr.quakenet.org 311 tesfa_ tesfa ~dd 5.195.225.158 * :H H]
        // [<hostname> 311 <requesterNick> <nick> <username> <ipadd> * :<real name>]
        std::string rpl_who = ":"+this->getServerHostName()+" 311 "+client->getNickName()+" "+whoClient->getNickName()+" "+whoClient->getUserName()+" "+whoClient->getIpAddr()+" * :"+whoClient->getRealName()+"\r\n";
        this->sendMsgToClient(client->getFd(), rpl_who);
    }
    if (command->cmd == "kill") {
        std::string potencialServerOp = "@%" + client->getNickName();
        if (*(this->_serverOperators.lower_bound(potencialServerOp)) != potencialServerOp)
            this->sendMsgToClient(client->getFd(), ERR_NOPRIVILEGES(this->getServerHostName(), client->getNickName()));
        else {
            // Client *clToBeKilled = this->getClientByNick(command->params[0]);
            // this->removeClient(client->getFd());
            // we gonna remove a client From everywhere that he is involved in!!
        }
    }
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
    buffer[bytes_received] = '\0'; // Null-terminate the received data coz recv() doesnt 
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
            Command *command = new Command((*it));
            Client *client = this->_clients[clientFd];

            // so far all the commands I know has to have at least one parameter!
            if (command->params.empty())
            {
                this->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(this->getServerHostName(), command->cmd));
                return ;
            }
            if (client->IsClientConnected() == false) {
                this->registerClient(client, command);
            }
            else
                this->doStuff(client, command);
            delete command; // we dont need the command anymore
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}