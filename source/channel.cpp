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

Channel::Channel(std::string chanName) {
    this->_chanName = chanName;
    this->_chanKey = "";
    this->_chanLimit = 0;
    this->_chanTopic = "";
    this->_hasTopic = false;
}

Channel::~Channel(void) {
    std::cout << "Channel named [" << this->getName() << "] got deleted!\n";
}

std::string Channel::getName(void) const{
    return this->_chanName;
}
size_t Channel::getNumOfChanMembers(void) const {
    return this->_members.size();
}

std::string     Channel::getChanKey(void) const {
    return (this->_chanKey);
}
void            Channel::setChanKey(std::string newKey) {
    this->_chanKey = newKey;
}

std::string    Channel::isClientaMember(std::string clientNick) const {
    std::string potentialOp = "@" + clientNick;
    std::set<std::string>::iterator it = this->_chanOps.find(potentialOp);
    if (it != this->_chanOps.end())
        return potentialOp;
    std::set<std::string>::iterator m_it = this->_members.find(clientNick);
    if (m_it != this->_members.end())
        return clientNick;
    return "";
}

void     Channel::deleteAMember(std::string victim) {
    this->_chanOps.erase("@" + victim);
    this->_members.erase(victim);
    this->_member_fd_map.erase(victim);
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
        else if (senderClient->getFd() != recvClient->getFd()) // #define PRIVMSG_RPLY(senderNick, senderUsername, clientIp, revieverNick, msg)
            serverInstance->sendMsgToClient(recvClient->getFd(), PRIVMSG_RPLY(senderNick, senderClient->getUserName(), senderClient->getIpAddr(), this->getName(), msg));
        recvClient = NULL;
    }
 }

 std::set<std::string>& Channel::getAllMembersNick() {
    return this->_members;
 }

 void       Channel::setUsersLimit(int num) {
    this->_chanLimit = static_cast<unsigned int>(num);
 }

std::set<char>&        Channel::getChannelModes() {
    return this->_chanModes;
}

bool                    Channel::isModeOn(char mode) {
    std::set<char>::iterator it = this->getChannelModes().find(mode);
    if (it == this->getChannelModes().end()) {
        return (false);
    }
    return true;
}

unsigned int       Channel::getUsersLimit(void) {
    return (this->_chanLimit);
}

std::set<std::string>&  Channel::getAllChanOps(void) {
    return (this->_chanOps);
}

std::set<std::string>&  Channel::getAllInvitees(void) {
    return (this->_invitedUser);
}

std::string             Channel::getTopic(void) const {
    return this->_chanTopic;
}
void          Channel::setTopic(std::string   newTopic) {
    this->_chanTopic = newTopic;
}

bool&        Channel::getHasTopic(void) {
    return (this->_hasTopic );
}