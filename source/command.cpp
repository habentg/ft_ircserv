/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 12:52:25 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/22 14:30:31 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

Command::Command(std::string rcved_cmd) {
    this->raw_cmd = rcved_cmd;
    std::vector<std::string> cmd_arr = split(rcved_cmd, ' ');
    std::vector<std::string>::iterator iter = cmd_arr.begin();
    this->cmd = cmd_arr[0];
    this->params.insert(this->params.end(), cmd_arr.begin() + 1, cmd_arr.end());
}

Command::~Command(void) {
    // std::cout << "Command Destructor Called!\n";
}

/* PASS */
void    Command::password(Client *client, Server* servInstance) {
    if (client->getIsAuthenticated()) {
        servInstance->sendMsgToClient(client->getClientFd(), ERR_ALREADYREGISTERED(servInstance->getServerHostName()));
        return ;
    }
    if (this->params.empty()) {
        servInstance->sendMsgToClient(client->getClientFd(), ERR_NEEDMOREPARAMS(servInstance->getServerHostName(), this->cmd));
        return ;
    }
    if (this->params[0] == servInstance->getPassword()) {
        client->setIsAuthenticated(true);
        std::cout << "Client: [" << client->getClientFd() << "] has authenticated\n";
    }
    else {
        servInstance->sendMsgToClient(client->getClientFd(), ERR_PASSWDMISMATCH(servInstance->getServerHostName()));
        if (client->getWrongPassCount() == 2) {
            std::cout << "Client: [" << client->getClientFd() << "] got removed because of 3 incorrect password attempts\n";
            servInstance->removeClient(client->getClientFd());
            return ;
        } else
            client->setWrongPassCount(client->getWrongPassCount() + 1);
    }
}

/* NICK*/
/* 
    Nicknames are non-empty strings with the following restrictions:

    -> They MUST NOT contain any of the following characters: space (' ', 0x20), comma (',', 0x2C), asterisk ('*', 0x2A),
        question mark ('?', 0x3F), exclamation mark ('!', 0x21), at sign ('@', 0x40).
    -> They MUST NOT start with any of the following characters: dollar ('$', 0x24), colon (':', 0x3A).
    -> They MUST NOT start with a character listed as a channel type, channel membership prefix, or prefix listed in the 
        IRCv3 multi-prefix Extension.
    -> They SHOULD NOT contain any dot character ('.', 0x2E).
    
    # Names of IRC entities (clients, servers, channels) are casemapped. 
        This prevents, for example, someone having the nickname 'Dan' and someone else having the nickname 'dan', 
        confusing other users.
    
    Servers MAY have additional implementation-specific nickname restrictions and SHOULD avoid the use of nicknames 
        which are ambiguous with commands or command parameters where this could lead to confusion or error.
*/
bool validNickName(std::vector<std::string> nick_params, int clientFd, Server* servInstance) {
    if (nick_params.empty()){
        servInstance->sendMsgToClient(clientFd, ERR_NEEDMOREPARAMS(servInstance->getServerHostName(), "NICK"));
        return false;
    }
    std::string nick = nick_params[0];
    if (nick_params.size() != 1 || nick.find(',') != std::string::npos || nick.find('?') != std::string::npos || nick.find('@') != std::string::npos || \
            nick.find('!') != std::string::npos || nick.find('*') != std::string::npos || nick.find('.') != std::string::npos) {
        servInstance->sendMsgToClient(clientFd, ERR_ERRONEUSNICKNAME(servInstance->getServerHostName()));
        return false;
    }
    if (nick[0] == '$' || nick[0] == ':' || std::isdigit(nick[0])) {
        servInstance->sendMsgToClient(clientFd, ERR_ERRONEUSNICKNAME(servInstance->getServerHostName()));
        return false;
    }
    return true;
}

bool    Command::nickname(Client *client, Server* servInstance) {
    if (validNickName(this->params, client->getClientFd(), servInstance) == false)
        return false;
    std::string nick = lowerCaseString(this->params[0]);
    if (servInstance->isNickNamDuplicate(client->getClientFd(), nick) == true) {
        servInstance->sendMsgToClient(client->getClientFd(), ERR_NICKNAMEINUSE(servInstance->getServerHostName(), client->getNICK()));
        return false;
    }
    client->setNICK(this->params[0]);
    std::cout << "Client: " << client->getClientFd() << " has set his nickname to [" << client->getNICK() << "]\n";
    return true;
}