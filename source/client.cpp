/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 02:24:22 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/21 19:15:18 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

Client::Client(int clientFd, struct sockaddr *clientInfo){
        this->_clientFd = clientFd;
        this->_isRegistered = false;
        this->_isAuthenticated = false;
        this->_NICK = "";
        this->_USERNAME = "";
        this->_REALNAME = "";
        this->_HOSTNAME = "";
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
bool Client::getIsregistered(void) const {
    return this->_isRegistered;
}
void Client::setIsregistered(bool value) {
    this->_isRegistered = value;
}

std::string Client::getNICK(void) const {
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

int Client::getClientFd(void) const {
    return this->_clientFd;
}

std::string Client::getHostName(void) const {
    return this->_HOSTNAME;
}
void Client::setHostName(std::string value) {
    this->_HOSTNAME = value;
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