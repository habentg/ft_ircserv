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
void    Command::password(Client *client, Server* serverInstance) {
    if (client->getIsAuthenticated()) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_ALREADYREGISTERED(serverInstance->getServerHostName()));
        return ;
    }
    if (this->params[0] == serverInstance->getPassword()) {
        client->setIsAuthenticated(true);
        std::cout << "Client: [" << client->getFd() << "] has been authenticated\n";
    }
    else {
        serverInstance->sendMsgToClient(client->getFd(), ERR_PASSWDMISMATCH(serverInstance->getServerHostName()));
        if (client->getWrongPassCount() == 2) {
            serverInstance->removeClient(client->getFd());
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
size_t validNickName(std::vector<std::string> nick_params, int clientFd, Server* serverInstance, std::string cmdName) {
    std::string nick = nick_params[0];
    if (std::isdigit(nick[0]) || nick[0] == ':' || nick[0] == '#') {
        serverInstance->sendMsgToClient(clientFd, ERR_ERRONEUSNICKNAME(serverInstance->getServerHostName()));
        return 0;
    }
    size_t  valid_index = 0;
    if ((valid_index = nick.find(',')) != std::string::npos || (valid_index = nick.find('?')) != std::string::npos || \
        (valid_index = nick.find('@')) != std::string::npos || (valid_index = nick.find('!')) != std::string::npos || \
        (valid_index = nick.find('*')) != std::string::npos || (valid_index = nick.find('.')) != std::string::npos || \
        (valid_index = nick.find('$')) != std::string::npos) {
            if (valid_index == 0) {
                serverInstance->sendMsgToClient(clientFd, ERR_ERRONEUSNICKNAME(serverInstance->getServerHostName()));
                return 0;
            }
        return valid_index;
    }
    return -1;
}

bool    Command::nickname(Client *client, Server* serverInstance) {
    size_t validCharIndex = validNickName(this->params, client->getFd(), serverInstance, this->cmd);
    if (validCharIndex == 0)
        return false;
    std::string nick_name = this->params[0].substr(0, validCharIndex);
    if (serverInstance->isClientAvailable(client->getFd(), nick_name) != 0) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NICKNAMEINUSE(serverInstance->getServerHostName(), nick_name));
        return false;
    }
    client->setNICK(nick_name);
    std::cout << "Client: " << client->getFd() << " has set his nickname to [" << client->getNickName() << "]\n";
    return true;
}

/* USER Command validation */
void    Command::user(Client *client, Server* serverInstance) {
    if (this->params[0].find(':') == std::string::npos) {
        std::string userName = "~" + this->params[0];
        client->setUserName(userName);
    }
    else {
        client->setUserName(client->getNickName());
        if(this->raw_cmd.find(':') != std::string::npos)
            client->setRealName(this->raw_cmd.substr(this->raw_cmd.find(':')));
    }
}

/* PRIVMSG has to be sent in this format:
    -> {:d__!~dd@5.195.225.158 PRIVMSG d___ :yea this is me}
    -> {:nick!username@hostname PRIVMSG nick :<message>}
*/
void Command::privmsg(Client *senderClient, Server *serverInstance) {
    if (this->raw_cmd.find(':') == std::string::npos) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOTEXTTOSEND(serverInstance->getServerHostName()));
        return ;
    }
    // if (isChannelName(this->params[0]) == true)
    //     sendToChannel();
    int recieverFd = serverInstance->isClientAvailable(senderClient->getFd(), this->params[0]);
    if (recieverFd == 0) {
        serverInstance->sendMsgToClient(recieverFd, ERR_NOSUCHNICK(serverInstance->getServerHostName(), this->params[0]));
        return ;
    }
    serverInstance->sendMsgToClient(recieverFd, serverInstance->constructReplayMsg(senderClient, this, this->params[0]));
}


void    Command::join(Client *client, Server *serverInstance) {
    std::string validChanName;
    if (this->params[0][0] != '#')
        validChanName = std::string("#" + this->params[0]);
    else 
        validChanName = this->params[0];
    // if that channel doesnt exist:
        // create channel, and add dude as an OP
    std::string channelKey = "";
    if (this->params.size() >= 2)
        channelKey = this->params[1];
    if (serverInstance->numberOfChannels() == 0 || serverInstance->doesChanExist(validChanName) == false)
    {
        serverInstance->createChannel(validChanName, channelKey, client);
        std::cout << "######> we have : " << serverInstance->numberOfChannels() << "CHANNELS#########\n";
        return ;
    }
    // if that channel exists:
        // add him, if he is not already there
    Channel *chann = serverInstance->getChanByName(validChanName);
    std::cout << "===> chann users: " << chann->getNumOfChanMembers() << "\n";
    if (chann->getChanKey() != channelKey)
        std::cout << "channel key mismatch\n";
    chann->addMember(client->getNickName());
    std::cout << "===> chann users: " << chann->getNumOfChanMembers() << "\n";
}