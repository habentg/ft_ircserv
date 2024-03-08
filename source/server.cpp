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
    std::cout << "Server created!" << std::endl;
}

Server& Server::createServerInstance(unsigned short int portNumber, std::string password) {
    if (!serverInstance)
        serverInstance = new Server(portNumber, password);
    return *serverInstance;
}

Server::~Server() {
    std::map<int, Client *>::iterator it = this->_clients.begin();
    std::map<int, Client *>::iterator t_it; 
    while (it != this->_clients.end()) {
        t_it = it++;
        Client *client = t_it->second;
        if (client != NULL) {
            this->removeClient(client, Conn_closed(getServerHostName()));
            t_it->second = NULL; // Avoid dangling pointer
        }
    }
    close(this->_serverSocketFd);
    std::cout << "Server Destructor Called!\n";
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
std::string Server::getServerHostName(void) const {
    return this->_serverHostName;
}

size_t    Server::getNumberOfClients(void) const {
    return this->_clients.size();
}

Client*   Server::getClientByNick(std::string nick) {
    std::string validNick = nick;
    if (nick[0] == '@')
        validNick = nick.substr(1);
    int clFd = this->isClientAvailable(validNick);
    if (clFd == 0)
        return NULL;
    std::map<int, Client *>::iterator it = this->_clients.lower_bound(clFd);
    return ((*it).second);
}

std::string Server::isClientServerOp(std::string nick) {
    std::set<std::string>::iterator it = this->_serverOperators.lower_bound("@%" + nick);
    if (it == this->_serverOperators.end() || (*it).substr(2) != nick)
        return "";
    return (*it);
}

/* ------------------------------------------------------------------------------------------ */
/*                           Creating Socket and Listening                                    */
/* ------------------------------------------------------------------------------------------ */
void Server::server_listen_setup(char *portNumber) {
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
    int status = getaddrinfo(NULL, portNumber, &hints, &res);
    if (status != 0)
        throw std::runtime_error(gai_strerror(status));
    if (res == NULL)
        throw std::runtime_error("Error: Address resolution result is NULL");
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) 
        throw std::runtime_error("Error: creating socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enabler, sizeof(int)))
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
    if (newfd < 0) // wont happen but protection
        return ;
    // also initialize all of them for conditional jumps avoidance
    struct pollfd newConFdStruct;
    newConFdStruct.fd = newfd;
    newConFdStruct.events = POLLIN; // event to watch for is POLLIN
    newConFdStruct.revents = 0; // revents is set to 0
    this->getFdArray().push_back(newConFdStruct);
}

int    Server::isClientAvailable(std::string nick) const {
    std::map<std::string, int>::const_iterator map_it = this->_nick_fd_map.lower_bound(nick);
    if (map_it == this->_nick_fd_map.end() || (*map_it).first != nick)
        return 0;
    return ((*map_it).second);
}



/* ------------------------------------------------------------------------------------------ */
/*                 Recive a message from an existing connection                               */
/* ------------------------------------------------------------------------------------------ */

void Server::sendMsgToAllClients(std::string conCloseNotice) {
    std::map<int, Client *>::iterator it = this->_clients.begin();
    for (; it != this->_clients.end(); it++)
        this->sendMsgToClient(it->first, conCloseNotice);
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
std::string Server::constructReplayMsg(std::string senderNick, Client *senderClient, std::string recieverNick, std::string msgToSend) {
    // std::string msg = cmd->raw_cmd.substr(cmd->raw_cmd.find(':') + 1);
    std::string rply = PRIVMSG_RPLY(senderNick, senderClient->getUserName(), this->getServerHostName(), recieverNick, msgToSend);
    return rply;
}

/* Removing a Client and Closing connection
    > remove the client from everything he is involved to (IMPORTANT!)
    > distruct the object
    > close the fd.
*/
void Server::removeClient(Client *cl, std::string quitMsg) {
    // we have to find a way to delete a client from the channels he is in
    std::set<std::string> chansInvolved = cl->getChannelsJoined();
    std::set<std::string>::iterator chanIt = chansInvolved.begin();
    for (; chanIt != chansInvolved.end(); ++chanIt) {
        Channel *chan = this->getChanByName(*chanIt);
        if (chan == NULL)
            continue ;
        /* this NOTICE has to be sent to all the channels he was in
            06:59 -!- tesfa [~dd@5.195.225.158] has quit [Quit: leaving]
        */
        this->sendMessageToChan(chan, cl->getNickName(), quitMsg, true);
        this->removeClientFromChan(cl->getNickName(), chan);
        chan = NULL;
    }
    // remove the fd STRUCT from the array as weelllllllll
    std::vector<struct pollfd>::iterator it = this->_fdsArray.begin();
    for (; it != this->_fdsArray.end(); it++) {
        if ((*it).fd == cl->getFd()) {
            this->_fdsArray.erase(it);
            break ;
        }
    }
    this->_clients.erase(cl->getFd()); // remove the client from the map
    this->_nick_fd_map.erase(cl->getNickName());
    delete cl; // delete the client object
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
    Channel* newChan = new Channel(chanName, creator);
    this->_channels.insert(std::make_pair(chanName, newChan));
    newChan->getAllChanOps().insert(("@" + creator->getNickName()));
    newChan->insertToMemberFdMap(("@" + creator->getNickName()), creator->getFd()); // he a chanOp!
    newChan->addMember(("@" + creator->getNickName())); // he a chanOp!
    creator->addChannelNameToCollection(newChan->getChannelName());
    std::cout << "CHANNEL : {" << newChan->getChannelName() << "} created!\n";
    /* 
        << JOIN #lef
        >> :tesfa___!~dd@5.195.225.158 JOIN #lef
        >> :hostsailor.ro.quakenet.org 353 tesfa___ = #lef :@tesfa___
        >> :hostsailor.ro.quakenet.org 366 tesfa___ #lef :End of /NAMES list.
    */
   this->sendMsgToClient(creator->getFd(), RPL_JOIN(creator->getNickName(), creator->getUserName(), creator->getIpAddr(), chanName));
   command->names(creator, this);
}

Channel   *Server::getChanByName(std::string chanName) {
    if (this->_channels.size() == 0) {
        return NULL;
    }
    std::map<std::string, Channel *>::iterator it = this->_channels.lower_bound(chanName);
    if (it == this->_channels.end() || (*it).first != chanName)
        return NULL;
    return ((*it).second);
}

 void      Server::removeClientFromChan(std::string victimNick, Channel *chan) {
    // delete a user from the channel
    chan->deleteAMember(victimNick);
    // (channel seize to exist if there is no user)
    if (chan->getNumOfChanMembers() == 0) {
        serverInstance->deleteAChannel(chan);
        std::cout << "Channel deleted coz no one is here\n";
    }
 }

void    Server::deleteAChannel(Channel *chan) {
    this->_channels.erase(chan->getChannelName()); // remove it from the map
    delete chan; // destruct it
}

void       Server::sendMessageToChan(Channel *chan, std::string sender, std::string msg, bool chanNotice) {
    // we have nick-fd table
    chan->sendToAllMembers(this, sender, msg, chanNotice);
}

/* ================================================================================================================== */
/*                                                  DO STUFF                                                          */
/* ================================================================================================================== */

void    Server::channelRelatedOperations(Client* client, Command *command) {
    if (command->cmd == "JOIN")
        command->join(client, this);
    else if (command->cmd == "KICK")
        command->kick(client, this);
    else if (command->cmd == "NAMES")
        command->names(client, this);
    else if (command->cmd == "PART")
        command->partLeavChan(client, this);
    else if (command->cmd == "MODE")
        command->mode(client, this); 
    else if (command->cmd == "INVITE")
        command->invite(client, this); 
}

/* Connected Client can DO these stuff basicly:
    =!> PONG
        - servers response when a client checks for livelyness of the server (no bigie)
    =!> PRIVMSG
        - clients can message privately between one another or send messag to a channel thier in
    =!> PRIVMSG
    
 */
void Server::doStuff(Client* client, Command *command) {
    if (command->cmd == "PING") {
        this->sendMsgToClient(client->getFd(), PONG(this->getServerHostName()));
        return ;
    }
    std::cout << "--> Full-CMD: {" << command->raw_cmd << "} <--\n";
    if (command->cmd == "PRIVMSG") {
        command->privmsg(client, this);
        return ;
    }
    if (command->cmd == "QUIT") {
        command->quit(client, this);
        return ;
    }
    /* Channel and channel related features --- big job */
    if (command->cmd == "JOIN" || command->cmd == "KICK" || command->cmd == "NAMES" || command->cmd == "MODE" || command->cmd == "PART" || command->cmd == "INVITE" || command->cmd == "TOPIC") {
        this->channelRelatedOperations(client, command);
        return ;
    }
    /* Not mandatory Commands */
    if (command->cmd == "WHOIS") {
        Client *whoClient = this->getClientByNick(command->params[0]);
        if (whoClient == NULL) {
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
            // check if the killer is an ircOp.
            std::string ircOp = isClientServerOp(client->getNickName());
            if (ircOp == "") {
                std::cout << "you are not server OP\n";
                return ;
            }
            // check if the victim is actually in the server
            int victimFd = isClientAvailable(command->params[0]);
            if (victimFd == 0) {
                std::cout << "victim is not in server to be killed\n";
                return ;
            }
            // remove client
            Client *victim = this->getClientByNick(command->params[0]);
            std::string killMsg = std::string("KILLED YA ......\r\n");
            this->removeClient(victim, killMsg);
            std::cout << killMsg << std::endl;
            /* FOR NOW HE CAN ALSO KILL HIMSELF */
        }
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
    if (bytes_received < 1) {
        std::cout << "this is the bytes recived: " << bytes_received << std::endl;
        return ;
    }
    buffer[bytes_received] = '\0'; // Null-terminate the received data coz recv() doesnt
    client->getRecivedBuffer() += std::string(buffer);
    if (std::strchr(client->getRecivedBuffer().c_str(), '\n') == NULL) {
        return ;
    }
    if (client->getRecivedBuffer().size() == 0)
        return ;
    try
    {
        std::vector<std::string> arr_of_cmds = split(client->getRecivedBuffer(), '\0');
        client->getRecivedBuffer() = ""; // empty the buffer;
        std::vector<std::string>::iterator it = arr_of_cmds.begin();
        for (; it != arr_of_cmds.end(); ++it) {
            Command *command = new Command((*it));

            // so far all the commands I know has to have at least one parameter!
                // we can have empty "USER" cmd i think
            if (command->params.empty() && command->cmd != "USER")
            {
                this->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(this->getServerHostName(), command->cmd));
                return ;
            }
            // we register the client first, (basicly assign nickname, username and other identifiers to the new connection so it can interact with other users)
            if (client->IsClientConnected() == false) {
                this->registerClient(client, command);
            }
            else // now he can do anything
                this->doStuff(client, command);
            delete command; // we dont need the command anymore
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}