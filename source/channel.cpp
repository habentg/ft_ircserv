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

Channel::Channel(std::string chanName, Client *creator) {
    this->_chanName = chanName;
    this->_chanKey = "";
    this->_creator = creator->getNickName();
}

Channel::~Channel(void) {
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

void            Channel::makeClientChanOp(std::string clientNick) {
    this->_chanOps.insert("@" + clientNick);
    std::cout << "--> [" +clientNick+"] is a chanOp now!\n";
    // I will see how to display this modified nick name on the client side (related to mode maybe)
}

std::string    Channel::isClientChanOp(std::string clientNick) const {
    std::string potentialOp = "@" + clientNick;
    std::set<std::string>::iterator it = this->_chanOps.lower_bound(potentialOp);
    if ((*it) != potentialOp)
        return "";
    return potentialOp;
}
std::string    Channel::isClientaMember(std::string clientNick) const {
    std::string potentialOp = "@" + clientNick;
    std::set<std::string>::iterator it = this->_members.lower_bound(potentialOp);
    if ((*it) == potentialOp)
        return potentialOp;
    std::set<std::string>::iterator m_it = this->_members.lower_bound(clientNick);
    if ((*m_it) == clientNick)
        return clientNick;
    return "";
}

void     Channel::deleteAMember(std::string victim) {
    std::string vic = this->isClientaMember(victim);
    if (vic == "")
        return ;
    this->_members.erase(vic);
    this->_member_fd_map.erase(vic);
}

void    Channel::insertToMemberFdMap(std::string nick, int fd) {
    this->_member_fd_map.insert(std::make_pair(nick, fd));
}

 void            Channel::sendToAllMembers(Server *serverInstance, std::string senderNick, std::string msg, bool chanNotice) {
    Client *senderClient = serverInstance->getClientByNick(senderNick);
    if (senderClient == NULL) {
        return ;
    }
    std::map<std::string, int>::iterator m_it = this->_member_fd_map.begin();
    for (; m_it != this->_member_fd_map.end(); ++m_it) {
        Client *recvClient = serverInstance->getClientByNick((*m_it).first);
        if (chanNotice == true)
            serverInstance->sendMsgToClient(recvClient->getFd(), msg);
        else if (senderClient->getFd() != recvClient->getFd())
            serverInstance->sendMsgToClient(recvClient->getFd(), serverInstance->constructReplayMsg(senderNick, senderClient, this->getChannelName(), msg));
        recvClient = NULL;
    }
 }