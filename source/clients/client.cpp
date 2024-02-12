/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/11 02:24:22 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/12 09:05:06 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

Client::Client(int clientFd, struct sockaddr *clientInfo)
    : _clientFd(clientFd), _isAuthenticated(false) ,_NICK(""), _NAME(""){
        this->_ipaddr = inet_ntoa(((struct sockaddr_in *)clientInfo)->sin_addr);
        this->_port = ntohs(((struct sockaddr_in *)clientInfo)->sin_port);
        this->_clientInfo = clientInfo;
    std::cout << "Client Default constructor called!\n";
}

Client::~Client(void) {
    std::cout << "Client Destructor called!\n";
}

bool Client::getIsAuthenticated(void) const {
    return this->_isAuthenticated;
}
