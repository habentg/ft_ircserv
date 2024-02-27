/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 02:24:22 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/22 13:00:37 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

Client::Client(int clientFd, struct sockaddr *clientInfo){
        this->_clientFd = clientFd;
        this->_isConnected = false;
        this->_isAuthenticated = false;
        this->_NICK = "";
        this->_USERNAME = "";
        this->_REALNAME = "42Real Name";
        this->_ipaddr = inet_ntoa(((struct sockaddr_in *)clientInfo)->sin_addr);
        this->_port = ntohs(((struct sockaddr_in *)clientInfo)->sin_port);
        this->_clientInfo = clientInfo;
        this->_wrongPassCount = 0;
    // std::cout << "Client Default constructor called!\n";
}

Client::~Client(void) {
    // std::cout << "Client Destructor called!\n";
}


bool Client::getIsAuthenticated(void) const {
    return this->_isAuthenticated;
}
void Client::setIsAuthenticated(bool value) {
    this->_isAuthenticated = value;
}
bool Client::IsClientConnected(void) const {
    return this->_isConnected;
}
void Client::setIsregistered(bool value) {
    this->_isConnected = value;
}

std::string Client::getNickName(void) const {
    return this->_NICK;
}
void Client::setNICK(std::string value) {
    this->_NICK = value;
}

std::string Client::getUserName(void) const {
    return this->_USERNAME;
}
void Client::setUserName(std::string value) {
    this->_USERNAME = value;
}

int Client::getFd(void) const {
    return this->_clientFd;
}

std::string Client::getRealName(void) const {
    return this->_REALNAME;
}
void Client::setRealName(std::string value) {
    this->_REALNAME = value;
}

std::string Client::getIpAddr(void) const {
    return std::string(this->_ipaddr);
}

unsigned int Client::getWrongPassCount(void) const {
    return this->_wrongPassCount;
}

void Client::setWrongPassCount(unsigned int value) {
    this->_wrongPassCount = value;
}

void      Client::removeClientFromChan(std::string victim, Server *serverInstance, Channel *chan) {
    // delete a user from the channel
    chan->deleteAMember(victim);
    // (channel seize to exist if there is no user)
    if (chan->getNumOfChanMembers() == 0)
        serverInstance->deleteAChannel(chan);
}
