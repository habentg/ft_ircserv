/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_connnection_registration.cpp                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 12:54:51 by hatesfam          #+#    #+#             */
/*   Updated: 2024/03/07 17:37:11 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

/* ------------------------------------------------------------------------------------------ */
/*                           accepting new connection                                         */
/* ------------------------------------------------------------------------------------------ */
/* 
    -> using "accept" function, we accept incomming connection request and record that fd.
    -> in the same function, we we "hold" all the client info in a storage structure.
    -> we add that new fd to our array of fds, so we can receive data form it. (gonna be an ID of that specific connection)
    -> we creat a new client with that fd and information! 
 */
void Server::acceptNewConnection(void) {
    struct sockaddr_storage client_addr;
    socklen_t client_addr_size = sizeof(client_addr);
    
    int newClientFd = accept(this->_serverSocketFd, (sockaddr *)&client_addr, &client_addr_size);
    if (newClientFd == -1) {
        throw std::runtime_error("Error: accepting connections!");
    }
    this->addToFdArray(newClientFd);
    
    struct sockaddr *clientInfo = (struct sockaddr *)&client_addr;
    
    Client *client = new Client(newClientFd, clientInfo);
    
    this->_clients.insert(std::pair<int, Client *>(newClientFd, client));
    std::cout << "New Client accepted: " << inet_ntoa(((struct sockaddr_in *)clientInfo)->sin_addr)
        << ":" << ntohs(((struct sockaddr_in *)clientInfo)->sin_port) << " at fd: " << newClientFd <<std::endl;
}

/* ------------------------------------------------------------------------------------------ */
/*                                  user registration                                         */
/* ------------------------------------------------------------------------------------------ */

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

/* USER authentication and offically registering them to our server:
    => password, nickname, username and Real Name (optional) validation
    => if server has password, only if the user provide a password should the other commands be validated!
    => after password match, uniqueness of NICKname should be confirmed (if not remind the client the provided nickname is taken).
    => only if the above parameters are met:
        -- we connect user.
        -- we send wellcome messages with other userfull(mandatory) information about the server and rules.
        -- MOTD 

*/
void Server::userAuthenticationAndWelcome(Client* client, Command *command) {
    // authenticate
    if (authenticateClient(client, command) == false)
        return ;
        // // send welcome message
    if (client->getIsAuthenticated() && client->getUserName() != "" && client->getNickName() != "") {
        client->setIsConnected(true);
        this->_nick_fd_map.insert(std::pair<std::string, int>(client->getNickName(), client->getFd()));
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
        std::cout << "Client: " << client->getFd() << " is Connected!!\n";
    }
}

/* Client Registration:
    -> Capability negotation - negotiation between the client and server on what the server is capable of
    -> Client sends CAP command:
        +> with "LS" parameter - to discover the availabe capabilities
        +> with "REQ" parameter - to request new capabilites
        +> with "END" parameters - to end the capability negotiation to continue with the regestration process 
*/
void Server::registerClient(Client *client, Command *command) {

    if (command->cmd == "CAP") {
        if (command->params[0] == "LS")
            this->sendMsgToClient(client->getFd(), CAP_LS_RESP(this->getServerHostName())); //check those (hard-coded responses)
        else if (command->params[0] == "REQ")
            this->sendMsgToClient(client->getFd(), CAP_ACK_RESP(this->getServerHostName()));
    }
    else {
        this->userAuthenticationAndWelcome(client, command);
    }
}