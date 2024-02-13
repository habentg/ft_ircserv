/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 02:24:22 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/13 09:38:57 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Client::Client(int clientFd, struct sockaddr *clientInfo)
    : _clientFd(clientFd),_isRegistered(false), _isAuthenticated(false) ,_NICK(""), _USERNAME(""){
        this->_ipaddr = inet_ntoa(((struct sockaddr_in *)clientInfo)->sin_addr);
        this->_port = ntohs(((struct sockaddr_in *)clientInfo)->sin_port);
        this->_clientInfo = clientInfo;
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
