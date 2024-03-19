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
Server* Server::serverInstance = NULL; // global serverInstance pointer, it will act as a flag if a single server instace is created!

Server::Server(unsigned short int  portNumber, std::string password) : _serverPort(portNumber) {
    this->_serverSocketFd = 0;
    this->_serverPassword = password;
    this->_serverHostName = "";
    std::cout << "Server started!" << std::endl;
}

Server *Server::createServerInstance(unsigned short int portNumber, std::string password) {
    if (!serverInstance) {
        serverInstance = new Server(portNumber, password);
        if (serverInstance == NULL)
            std::cerr << "[ Failed to allocate memory for server!!]\n";
    }
    return serverInstance;
}

Server::~Server() {
    std::map<int, Client *>::iterator it = this->_clients.begin();
    std::map<int, Client *>::iterator t_it; 
    while (it != this->_clients.end()) {
        t_it = it++;
        Client *client = t_it->second;
        if (client != NULL) {
            this->sendMsgToClient(client->getFd(), Conn_closed(this->getHostname()));
            this->removeClient(client, Conn_closed(this->getHostname()));
        }
    }
    this->_clients.clear();
    close(this->_serverSocketFd);
    std::cout << "Server shut down!\n";
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
    std::map<int, Client *>::iterator it = this->_clients.find(clientFd);
    if (it == this->_clients.end())
        return NULL;
    return this->_clients[clientFd];
}

std::string Server::getPassword(void) const {
    return this->_serverPassword;
}
std::string Server::getHostname(void) const {
    return this->_serverHostName;
}

size_t    Server::getNumberOfClients(void) const {
    return this->_clients.size();
}

Client*   Server::getClientByNick(std::string nick) {
    std::string validNick = nick;
    if (nick[0] == '@')
        validNick = nick.substr(1);
    int clFd = this->getClientFd(validNick);
    if (clFd == 0)
        return NULL;
    std::map<int, Client *>::iterator it = this->_clients.find(clFd);
    return ((*it).second);
}

/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */
void Server::server_listen_setup(char *service) {
    int sockfd;
    int enabler = 1; // for setsockopt() -> to avoid "is already in use" error
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
    int status = getaddrinfo(NULL, service, &hints, &res);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    if (res == NULL)
        throw std::runtime_error("Error: Address resolution result is NULL");
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Error: creating socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enabler, sizeof(int)))
        throw std::runtime_error("Error: setsockopt(): " + std::string(strerror(errno)));
    if (fcntl(sockfd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error: fcntl(): " + std::string(strerror(errno)));
    if (bind(sockfd, res->ai_addr, res->ai_addrlen) < 0)
        throw std::runtime_error("Error: bind(): " + std::string(strerror(errno)));
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
    if (newfd < 0) // wont happen but protection
        return ;
    struct pollfd newConFdStruct;
    newConFdStruct.fd = newfd;
    newConFdStruct.events = POLLIN; // event to watch for is POLLIN
    newConFdStruct.revents = 0; // revents is set to 0
    this->getFdArray().push_back(newConFdStruct);
}

int    Server::getClientFd(std::string nick) const {
    std::map<std::string, int>::const_iterator map_it = this->_nick_fd_map.find(nick);
    if (map_it == this->_nick_fd_map.end())
        return 0;
    return ((*map_it).second);
}



/* ------------------------------------------------------------------------------------------ */
/*                 Recive a message from an existing connection                               */
/* ------------------------------------------------------------------------------------------ */

void Server::sendMsgToClient(int clientFd, std::string msg) {
    int bytes_sent = send(clientFd, msg.c_str(), msg.length(), 0);
    if (bytes_sent == -1)
        throw std::runtime_error("Error: sending message to client");
}

/* Removing a Client and Closing connection
    > remove the client from everything he is involved to (IMPORTANT!)
    > distruct the object
    > close the fd.
*/
void Server::removeClient(Client *cl, std::string quitMsg) {
    std::set<std::string> chansInvolved = cl->getChannelsJoined();
    std::set<std::string>::iterator chanIt = chansInvolved.begin();
    for (; chanIt != chansInvolved.end(); ++chanIt) {
        Channel *chan = this->getChannel(*chanIt);
        if (chan == NULL)
            continue ;
        if (chan->getAllMembersNick().size() > 1)
            this->forwardMsgToChan(chan, cl->getNickName(), quitMsg, true);
        this->removeClientFromChan(cl, chan);
        chan = NULL;
    }
    chansInvolved.clear();
    std::vector<struct pollfd>::iterator it = this->_fdsArray.begin();
    for (; it != this->_fdsArray.end(); it++) {
        if ((*it).fd == cl->getFd()) {
            this->_fdsArray.erase(it);
            break ;
        }
    }
    this->_clients.erase(cl->getFd());
    this->_nick_fd_map.erase(cl->getNickName());
    delete cl;
}

/* ================================================================================================================== */
/*                                      Channel related Operations                                                    */
/* ================================================================================================================== */

/* 
    // after creation of channel, these should be displayed on the channel window.
        06:32 -!- tesfa__ [~dd@5.195.225.158] has joined #ullaMo
        06:32 [Users #ullaMo]
        06:32 [@tesfa__]
        06:32 -!- Irssi: #ullaMo: Total of 1 nicks [1 ops, 0 halfops, 0 voices, 0 normal]
        06:32 -!- Channel #ullaMo created Tue Feb 27 06:32:49 2024
 */

void    Server::createChannel(std::string chanName, Client *creator, Command *command) {
    Channel* newChan = new Channel(chanName);
    this->_channels.insert(std::make_pair(chanName, newChan));
    newChan->getAllChanOps().insert(("@" + creator->getNickName()));
    newChan->insertToMemberFdMap((creator->getNickName()), creator->getFd());
    newChan->getAllMembersNick().insert((creator->getNickName()));
    creator->getChannelsJoined().insert(newChan->getName());
    std::cout << "CHANNEL : {" << newChan->getName() << "} created!\n";
   this->sendMsgToClient(creator->getFd(), RPL_JOIN(creator->getNickName(), creator->getUserName(), creator->getIpAddr(), chanName));
   this->namesCmd(creator, chanName); // send names
}

Channel   *Server::getChannel(std::string chanName) {
    if (this->_channels.empty())
        return NULL;
    std::map<std::string, Channel *>::iterator it = this->_channels.find(chanName);
    if (it == this->_channels.end()) {
        return NULL;
    }
    return ((*it).second);
}

 void      Server::removeClientFromChan(Client *victim, Channel *chan) {
    // delete a user from the channel
    chan->deleteAMember(victim->getNickName());
    victim->getChannelsJoined().erase(chan->getName());
    // (channel seize to exist if there is no user)
    if (chan->getNumOfChanMembers() == 0 && this->getChannel(chan->getName()) != NULL) {
        this->deleteAChannel(chan);
    }
 }

void    Server::deleteAChannel(Channel *chan) {
    this->_channels.erase(chan->getName()); // remove it from the map
    delete chan; // destruct it
}

void       Server::forwardMsgToChan(Channel *chan, std::string sender, std::string msg, bool isChanNotice) {
    // we have nick-fd table
    chan->sendToAllMembers(this, sender, msg, isChanNotice);
}

/* ================================================================================================================== */
/*                                                  DO STUFF                                                          */
/* ================================================================================================================== */

/* Connected Client can DO these stuff basicly:
    =!> PONG
        - servers response when a client checks for livelyness of the server (no bigie)
    =!> PRIVMSG
        - clients can message privately between one another or send messag to a channel thier in
    =!> PRIVMSG
    
 */
void Server::doStuff(Client* client, Command *command) {
    // instead of split repeatition, I devided the commands on whether they need to split the first parameter, second parameter or they dont need to split!
    if (command->cmd == "PING")
        this->sendMsgToClient(client->getFd(), PONG(this->getHostname()));
    if (command->cmd == "MODE") // no need to split
        command->mode(client, this);
    else if (command->cmd == "INVITE")
        command->invite(client, this);
    else if (command->cmd == "QUIT")
        command->quit(client, this);
    /* need to spit first parameter */
    else if (command->cmd == "PRIVMSG" || command->cmd == "NOTICE" || command->cmd == "NAMES" || command->cmd == "JOIN" || command->cmd == "PART") {
        std::vector<std::string> recievers = split(command->params[0], ',');
        for(std::vector<std::string>::iterator it = recievers.begin(); it != recievers.end(); ++it) {
            if (command->cmd == "PRIVMSG")
                command->privmsg(client, this, *it);
            else if (command->cmd == "NOTICE")
                command->notice(client, this, *it);
            else if (command->cmd == "NAMES")
                this->namesCmd(client, *it);
            else if (command->cmd == "JOIN")
                command->join(client, this, *it);
            else if (command->cmd == "PART")
                command->part(client, this, *it);
        }
    }
    /* need to split the second parameter */
    else if (command->cmd == "KICK") {
        if (command->params.size() < 2) {
            this->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(this->getHostname(), command->cmd));
            return ;
        }
        std::vector<std::string> victims = split(command->params[1], ',');
        for(std::vector<std::string>::iterator it = victims.begin(); it != victims.end(); ++it) {
            command->kick(client, this, *it);
        }
    }
    /* both prameters might need split */
    else if (command->cmd == "TOPIC") {
        if (command->params[0] == "-delete")
            command->unsetTopic(client, this);
        else {
            std::vector<std::string> channels = split(command->params[0], ',');
            for(std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
                command->topic(client, this, *it);
            }
        }
    }
}

void Server::executeMsg(Client *client) {
    std::vector<std::string> arr_of_cmds = split(client->getRecivedBuffer(), '\0');
    client->getRecivedBuffer().erase(); // empty the buffer - coz its splited and changed to CMD;
    for (std::vector<std::string>::iterator it = arr_of_cmds.begin(); it != arr_of_cmds.end(); ++it) {
        Command *command = new Command((*it));
        if (command->params.empty() && command->cmd != "USER") {
            this->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(this->getHostname(), command->cmd));
            return ;
        }
        // we register the client first, (basicly assign nickname, username and other identifiers to the new connection so it can interact with other users)
        if (client->IsClientConnected() == false)
            this->registerClient(client, command);
        else
            this->doStuff(client, command);
        delete command; // we dont need the command anymore
    }
}

void Server::recieveMsg(int clientFd) {
    char buffer[1024];
    int buffer_size = sizeof(buffer);

    Client *client = this->getClient(clientFd);
    if (client == NULL)
        return ;
    std::memset(buffer, 0, buffer_size);
    int bytes_received = recv(clientFd, buffer, buffer_size, 0);
    if (bytes_received == 0) {
        this->removeClient(client, Conn_closed(this->getHostname()));
        return ;
    }
    if (bytes_received < 0)
        return ;
    buffer[bytes_received] = '\0'; // Null-terminate the received data coz recv() doesnt
    client->getRecivedBuffer() += std::string(buffer);
    if (client->getRecivedBuffer().find("\n") == std::string::npos)
        return ;
    this->executeMsg(client);
}
