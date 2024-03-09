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
}

/* PASS authentication:
    # shoudnt be already authenticated.
    # should be only allowed 3 max tries, then close connection!
*/
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
            serverInstance->removeClient(client, std::string("3 wrong password attempt"));
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
    /* Use find first instead of all the '||'s */
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

/* NICK Command validation */
bool    Command::nickname(Client *client, Server* serverInstance) {
    size_t invalidCharIndex = validNickName(this->params, client->getFd(), serverInstance, this->cmd); // double check again
    if (invalidCharIndex == 0) // return only if the is not a single valid character
        return false;
    std::string nick_name = this->params[0].substr(0, invalidCharIndex); 
    if (serverInstance->isClientAvailable(nick_name) != 0) { // if a client with the same nickname is already available
        serverInstance->sendMsgToClient(client->getFd(), ERR_NICKNAMEINUSE(serverInstance->getServerHostName(), nick_name));
        return false;
    }
    client->setNICK(nick_name); // set nickname
    return true;
}

/* USER Command validation:
    Command: USER
    Parameters: <username> <hostname> <servername> :<real name>
        -> hostname and servername are ignored (are used in server-server communication for client advertising)
    <> if there is not real name, give it some difaault value,
*/
void    Command::user(Client *client, Server* serverInstance) {
    if (this->params.size() == 0) {
        std::string userName = "~" + client->getNickName(); // ~ to indicate it was server given
        client->setUserName(userName);
        return ;
    }
    if (client->getUserName() != "") {
        serverInstance->sendMsgToClient(client->getFd(), ERR_ALREADYREGISTERED(serverInstance->getServerHostName()));
        return ;   
    }
    client->setUserName(this->params[0]);
    if (this->params[0].find(':') != std::string::npos) {
        client->setRealName(this->raw_cmd.substr(this->raw_cmd.find(':')));
    }
}

/* PRIVMSG cmd:

    -> there should be a text to send.
    -> if the message is meant to be sent to channel:
        * that channel should exist.
        * sender should be in that channel.
    -> if it was for another client:
        # reviever should exist


has to be sent in this format:
    -> {:d__!~dd@5.195.225.158 PRIVMSG d___ :yea this is me}
    -> {:nick!username@hostname PRIVMSG nick :<message>}
*/
// big problem with nickname display thing !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
void Command::privmsg(Client *senderClient, Server *serverInstance) {
    if (this->raw_cmd.find(':') == std::string::npos) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOTEXTTOSEND(serverInstance->getServerHostName()));
        return ;
    }
    std::string msgPriv = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    if (this->params[0][0] == '#') {
        Channel *chan = serverInstance->getChanByName(this->params[0]);
        if (chan == NULL) {
            serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getServerHostName(), senderClient->getNickName(), chan->getName()));
            return ;
        }
        std::string sender = chan->isClientaMember(senderClient->getNickName());
        if (sender == "") {
            /* or we can send "404": cant send to channel, the below format
                >> :stockholm.se.quakenet.org 404 tesfa_ #habex42ad :Cannot send to channel
            */
            serverInstance->sendMsgToClient(senderClient->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getServerHostName(), senderClient->getNickName(), chan->getName()));
            return ;
        }
        serverInstance->sendMessageToChan(chan, sender, msgPriv, false);
        return ;
    }
    int recieverFd = serverInstance->isClientAvailable(this->params[0]);
    if (recieverFd == 0) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOSUCHNICK(serverInstance->getServerHostName(), senderClient->getNickName(), this->params[0]));
        return ;
    }
    serverInstance->sendMsgToClient(recieverFd, serverInstance->constructReplayMsg(senderClient->getNickName(), senderClient, this->params[0], msgPriv));
}

/* Channel naming:
    >> Apart from the requirement of the first character being a valid channel type prefix character;
        the only restriction on a channel name is that it may not contain any spaces (' ', 0x20),
        a control G / BELL ('^G', 0x07), or a comma (',', 0x2C) (which is used as a list item separator by the protocol).
*/

void    Command::join(Client *client, Server *serverInstance) {
    if (this->params[0][0] != '#' || this->params[0].find(',') != std::string::npos || this->params[0].find(7) != std::string::npos) {
        // incase of a channel name having a space, we can reconstruct the "name" but I am lazy now!
        //{:adrift.sg.quakenet.org 403 tesfa #fhfjk :No such channel}
        serverInstance->sendMsgToClient(client->getFd(), ERR_BADCHANMASK(serverInstance->getServerHostName(), client->getNickName()));
        return ;
    }
    std::string validChanName = this->params[0];
    // if that channel doesnt exist:
        // create channel, and add dude as an OP
    Channel *chann = serverInstance->getChanByName(validChanName);
    if (chann == NULL)
    {
        serverInstance->createChannel(validChanName, client, this);
        return ;
    }
    // we check if channel is an invite only:
    if (chann->isModeOn('i') || (chann->getAllInvitees().find(client->getNickName()) != chann->getAllInvitees().end())) {
        // check if the user has an invitation
        std::set<std::string>::iterator it = chann->getAllInvitees().lower_bound(client->getNickName());
        if (it == chann->getAllInvitees().end() || (*it) != client->getNickName()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_INVITEONLYCHAN(serverInstance->getServerHostName(), client->getNickName(), chann->getName()));
            std::cout << "dude !!!!! you is not invited\n";
            return ;
        }
        if (chann->isModeOn('l') && chann->getNumOfChanMembers() == chann->getUsersLimit()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_CHANNELISFULL(serverInstance->getServerHostName(), client->getNickName(), chann->getName()));
            std::cout << "you was invited but ... channel is full\n";
            return ;
        }
        // if not return;
        // if yes (one time use of invite -- delema)
    }
    /* before joining him to channel -- we have to check if he has a correct key and if the user limit of the channel is reached (both of them if nessesaary) */
    else if ((chann->isModeOn('k') || chann->isModeOn('l')))
    {
        if (this->params.size() == 1 || (this->params[1] != chann->getChanKey())){// he came without key or there is key mismatch
            serverInstance->sendMsgToClient(client->getFd(), ERR_BADCHANNELKEY(serverInstance->getServerHostName(), client->getNickName(), chann->getName()));
            return ;
        }
        if (chann->isModeOn('l') && chann->getNumOfChanMembers() == chann->getUsersLimit()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_CHANNELISFULL(serverInstance->getServerHostName(), client->getNickName(), chann->getName()));
            return ;
        }
        // at this point he got the correct key and channel user limmit hasnt been reached yet
    }
    chann->addMember(client->getNickName());
    chann->insertToMemberFdMap(client->getNickName(), client->getFd());
    client->addChannelNameToCollection(chann->getName());
    std::string letThemKnow = RPL_JOIN(client->getNickName(), client->getUserName(), client->getIpAddr(), chann->getName());
    serverInstance->sendMessageToChan(chann, client->getNickName(), letThemKnow, true);
    this->names(client, serverInstance);
    /* 18:40 -!- Channel #42chan created Fri Mar  8 18:40:38 2024 */ // DO THIS
    /* >> :hostsailor.ro.quakenet.org 329 tom_ #sdff 1709910616 */
    // serverInstance->sendMsgToClient(client->getFd(), RPL_TIME(serverInstance->getServerHostName(), client->getNickName(), chann->getName(), std::string(getCurrentTime())));
    if (chann->isModeOn('i'))
        chann->getAllInvitees().erase(client->getNickName()); // invitation used and expired!!!

    std::cout << "Added {"<<client->getNickName()<<"} to <"<<chann->getName()<<"> has : " <<chann->getNumOfChanMembers()<<" members now\n";
}

/*
    Command: KICK
    Parameters: <channel> <user> *( "," <user> ) [<comment>]

    recieved format: {KICK #42 tesfa_ :he is real bad man}
 */
void    Command::kick(Client *senderClient, Server *serverInstance) {
    // check if channel exists; if not, send error and return!
    if (this->params.size() < 2) {
        std::cout << "not enough parameters for kick cmd\n";
        return ;
    }
    Channel *chan = serverInstance->getChanByName(this->params[0]);
    if (chan == NULL) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getServerHostName(), senderClient->getNickName(), this->params[0]));
        std::cout << "channel: [" << this->params[0] << "] doesnt exist!\n";
        return ;
    }
    // check if "kicker" is a chanOp, if not send error and return
    std::string chanOp = chan->isClientaMember(senderClient->getNickName());
    if (chanOp == "") {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getServerHostName(), senderClient->getNickName(), chan->getName()));
        std::cout << "sender user: [" << senderClient->getNickName() << "] is not an IN CHANNEL!\n";
        return ;
    }
    if (chanOp[0] != '@') {
        // notice in your side only that sais you are not OP!
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getServerHostName(), senderClient->getNickName(), chan->getName()));
        std::cout << "sender user: [" << senderClient->getNickName() << "] is not an OP!\n";
        return ;
    }
    // check if the user exists in said channel, if not send error and return
    std::string victim = chan->isClientaMember(this->params[1]);
    if (victim == "") {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_USERNOTINCHANNEL(serverInstance->getServerHostName(), senderClient->getNickName(), chan->getName()));
        std::cout << "victim user: [" << senderClient->getNickName() << "] is not in channel!\n";
        return ;
    }
    // remove him, send a message on why he is being kicked-out!
    // send a notice (with reason) that he is being kicked out from a channel
    std::cout << " >>>> Before users kick: [" << chan->getNumOfChanMembers() << "]\n";
    std::string kickMsg = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    if (kickMsg == "")
        kickMsg = victim;
    std::string kickNotice = RPL_KICK(senderClient->getNickName(), senderClient->getUserName(), senderClient->getIpAddr(), chan->getName(), victim, kickMsg);
    // inform all user of the channel that someone got kicked out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /* 
        << KICK #lef tesfa____ :
        >> :tesfa___!~dd@5.195.225.158 KICK #lef tesfa____ :tesfa___
        :htg!~dd@5.195.225.158 KICK #ab42chan tesfa :bye bigman
     */
    serverInstance->sendMessageToChan(chan, chanOp, kickNotice, true);
    serverInstance->removeClientFromChan(victim, chan);    
    std::cout << " >>>> After users kick: [" << chan->getNumOfChanMembers() << "]\n";
}


void    Command::quit(Client *senderClient, Server *serverInstance) {
    std::cout << "++ before remove: " << serverInstance->getNumberOfClients() << " left in server\n";

    std::string quitMsg = RPL_QUIT(senderClient->getNickName(), senderClient->getUserName(), senderClient->getIpAddr(), this->raw_cmd);

    serverInstance->removeClient(senderClient, quitMsg);
}

void    Command::partLeavChan(Client *senderClient, Server *serverInstance) {
    /* :htg_!~dd@5.195.225.158 PART #gele */
    Channel *chan = serverInstance->getChanByName(this->params[0]);
    if (chan == NULL)
        return ;
    std::string reason = std::string("Default Leaving Reason");
    if (this->raw_cmd.find(':') != std::string::npos)
        reason = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    std::string partMsg = RPL_PART(senderClient->getNickName(), senderClient->getUserName(), senderClient->getIpAddr(), chan->getName(), reason);
    serverInstance->sendMessageToChan(chan, senderClient->getNickName(), partMsg, true);
    serverInstance->removeClientFromChan(senderClient->getNickName(), chan);
    return ;
}

void Command::names(Client *client, Server *serverInstance) {
    // Find the channel object based on the channel name
    std::string channelName = this->params[0];
    Channel *channel = serverInstance->getChanByName(channelName);
    if (channel == NULL) { // If channel doesn't exist, send error message to client
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getServerHostName(), client->getNickName(), channelName));
        return;
    }
    // Get the list of nicknames
    // the Op's first
    std::set<std::string> opNicks = channel->getAllChanOps();
    std::string namesReply = RPL_NAMES(serverInstance->getServerHostName(), client->getNickName(), channelName);
    std::set<std::string>::iterator it = opNicks.begin();
    // Construct the NAMES reply message
    for (; it != opNicks.end(); ++it) {
        namesReply += (*it) + " ";
    }
    // then the normal members
    std::set<std::string> nicknames = channel->getAllMembersNick(); // I USED reference (des kbleka ile tomas)
    std::set<std::string>::iterator it_n = nicknames.begin();
    for (; it_n != nicknames.end(); ++it_n) {
        if (channel->isClientChanOp((*it_n)) == "") {
            namesReply += (*it_n) + " ";
        }
    }
    namesReply += "\r\n";
    // Send the NAMES reply message to the client
    serverInstance->sendMsgToClient(client->getFd(), namesReply);

    // Send the /END of NAMES reply message
    serverInstance->sendMsgToClient(client->getFd(), RPL_ENDOFNAMES(serverInstance->getServerHostName(), client->getNickName(), channelName));
}

/* 
    Command: INVITE
  Parameters: <nickname> <channel>

*/
void    Command::invite(Client *client, Server *serverInstance) {
    // error checking;
    if (this->params.size() < 2) {
        std::cout << "need more parms of INVITE\n";
        return ;
    }
    // checking channel availability and the channel is an invite-only
    Channel *chan = serverInstance->getChanByName(this->params[1]);
    if (chan == NULL) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getServerHostName(), client->getNickName(), chan->getName()));
        std::cout << "no such chanel for INVITE\n";
        return ;
    }
    /*  */
    // if (chan->getChannelModes().find('i') == chan->getChannelModes().end()) {
    //     std::cout << "channe is not an 'invite-only' for INVITE\n";
    //     return ;
    // }
    // checking is invitor is in channel and is actuall a chanOp
    std::string chanOp = chan->isClientaMember(client->getNickName());
    if (chanOp == "") {
        serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getServerHostName(), client->getNickName(), chan->getName()));
        std::cout << "invitor is not in channel for INVITE\n";
        return ;
    }
    if (chanOp[0] != '@') {
        serverInstance->sendMsgToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getServerHostName(), client->getNickName(), chan->getName()));
        std::cout << "invitor is not a ChanOp for INVITE\n";
        return ;
    }
    // checking invitee's availability in server, if truee we check if he already in channel;
    Client *invitee = serverInstance->getClientByNick(this->params[0]);
    if (invitee == NULL) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHNICK(serverInstance->getServerHostName(), client->getNickName(), this->params[0]));
        std::cout << "invitee is not in SERVER for INVITE\n";
        return ;
    }
    if (chan->isClientaMember(invitee->getNickName()) != "") {
        serverInstance->sendMsgToClient(client->getFd(), ERR_USERONCHANNEL(serverInstance->getServerHostName(), client->getNickName(), invitee->getNickName(), chan->getName()));
        std::cout << "dude is already in channel, he dont need no invite! for INVITE\n";
        return ;
    }
    // adding invitee's nick name in our record of invitee's (needed to be deleted after one time use)
    chan->getAllInvitees().insert(invitee->getNickName());
    serverInstance->sendMsgToClient(invitee->getFd(), RPL_YouIsInvited(client->getNickName(), client->getNickName(), client->getIpAddr(), chan->getName(), invitee->getNickName()));
    /* >> :afk!~hab@5.195.225.158 INVITE cfk #42chanasfaf */
}