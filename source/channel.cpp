/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:51:45 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/21 12:45:16 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

Channel::Channel(std::string chanName, std::string key, Client *creator) {
    this->_chanName = chanName;
    this->_chanKey = key;
    this->_creator = creator->getNickName();
    this->_members.insert(creator->getNickName());
    this->_operators.insert(creator->getNickName());
    std::cout << "channel created!\n";
}

Channel::~Channel(void) {
    std::cout << "channel Destructor Called!\n";
}

std::string Channel::getChannelName(void) const{
    return this->_chanName;
}
size_t Channel::getNumOfChanMembers(void) const {
    return this->_members.size();
}

void Channel::addMember(std::string clientNick) {
    this->_members.insert(clientNick);
}

void Channel::deleteMember(std::string clientNick) {
    this->_members.erase(clientNick);
}

bool Channel::doesClientAlreadyAMember(std::string clientNick) {
    std::set<std::string>::iterator m_it = this->_members.lower_bound(clientNick);
    if ((*m_it) != clientNick)
        return false;
    return true;
}


std::string     Channel::getChanKey(void) const {
    return (this->_chanKey);
}
void            Channel::setChanKey(std::string newKey) {
    this->_chanKey = newKey;
}