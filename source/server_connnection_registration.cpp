/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server_connnection_registration.cpp                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/28 12:54:51 by hatesfam          #+#    #+#             */
/*   Updated: 2024/03/19 05:09:55 by hatesfam         ###   ########.fr       */
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
    if (fcntl(newClientFd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error: fcntl from accept new connection");
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
        // provide nick
    }
    else
        return (this->sendMsgToClient(cl->getFd(), ERR_REGISTER_FIRST(this->getHostname())), false);
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
        this->sendMsgToClient(client->getFd(), RPL_WELCOME(this->getHostname(), client->getUserName(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_YOURHOST(this->getHostname(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_CREATED(this->getHostname(), client->getNickName(), humanReadableDateTime()));
        this->sendMsgToClient(client->getFd(), RPL_MYINFO(this->getHostname(), client->getNickName()));
        this->sendMsgToClient(client->getFd(), RPL_ISUPPORT(this->getHostname(), client->getNickName()));
        /* MOTD */
        this->sendMsgToClient(client->getFd(), RPL_MOTDSTART(this->getHostname(), client->getNickName()));
        // std::ifstream file("toFix.txt", std::ifstream::in);
        // if (!file.is_open())
        //     this->sendMsgToClient(client->getFd(), RPL_MOTD(this->getHostname(), client->getNickName(), "Coudnt Open MOTD File! Welcome anyway ....."));
        // else {
        //     std::string line;
        //     while (std::getline(file, line)) {
        //         this->sendMsgToClient(client->getFd(), RPL_MOTD(this->getHostname(), client->getNickName(), line));
        //     }
        //     file.close();
        // }
        std::ifstream file("MOTD.txt", std::ifstream::in);
        if (!file.is_open()) {
            std::cerr << "Error opening MOTD file!" << std::endl;
        } else {
            std::string line;
            while (std::getline(file, line)) {
                std::cerr << line << std::endl;
                this->sendMsgToClient(client->getFd(), RPL_MOTD(this->getHostname(), client->getNickName(), line));
            }
            file.close();
        }
        this->sendMsgToClient(client->getFd(), RPL_ENDOFMOTD(this->getHostname(), client->getNickName()));
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
            this->sendMsgToClient(client->getFd(), CAP_LS_RESP(this->getHostname())); //check those (hard-coded responses)
        else if (command->params[0] == "REQ")
            this->sendMsgToClient(client->getFd(), CAP_ACK_RESP(this->getHostname()));
    }
    else {
        this->userAuthenticationAndWelcome(client, command);
    }
}