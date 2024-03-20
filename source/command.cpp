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
        serverInstance->sendMsgToClient(client->getFd(), ERR_ALREADYREGISTERED(serverInstance->getHostname()));
        return ;
    }
    if (this->params[0] == serverInstance->getPassword()) {
        client->setIsAuthenticated(true);
        std::cout << "Client: [" << client->getFd() << "] has been authenticated\n";
    }
    else {
        serverInstance->sendMsgToClient(client->getFd(), ERR_PASSWDMISMATCH(serverInstance->getHostname()));
        if (client->getWrongPassCount() == 2) {
            serverInstance->removeClient(client, std::string("3 wrong password attempt\r\n"));
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
        serverInstance->sendMsgToClient(clientFd, ERR_ERRONEUSNICKNAME(serverInstance->getHostname()));
        return 0;
    }
    size_t  valid_index = 0;
    /* Use find first instead of all the '||'s */
    std::string check = std::string(",@*$?!.");
    if ((valid_index = nick.find_first_of(check)) != std::string::npos) {
        if (valid_index == 0) {
            serverInstance->sendMsgToClient(clientFd, ERR_ERRONEUSNICKNAME(serverInstance->getHostname()));
            return 0;
        }
        return valid_index;
    }
    return -1;
}

/* NICK Command validation */
/*
    * ames of IRC entities (clients, servers, channels) are casemapped.
    This prevents, for example, someone having the nickname 'Dan' and someone else having the nickname 'dan', confusing other users. 
    Servers MUST advertise the casemapping they use in the RPL_ISUPPORT numeric that’s sent when connection registration has completed.
*/
bool    Command::nickname(Client *client, Server* serverInstance) {
    size_t invalidCharIndex = validNickName(this->params, client->getFd(), serverInstance, this->cmd); // double check again
    if (invalidCharIndex == 0) // return only if the is not a single valid character
        return false;
    std::string nick_name = this->params[0].substr(0, invalidCharIndex); 
    if (serverInstance->getClientFd(nick_name) != 0) { // if a client with the same nickname is already available
        serverInstance->sendMsgToClient(client->getFd(), ERR_NICKNAMEINUSE(serverInstance->getHostname(), nick_name));
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
        serverInstance->sendMsgToClient(client->getFd(), ERR_ALREADYREGISTERED(serverInstance->getHostname()));
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
void Command::privmsg(Client *senderClient, Server *serverInstance, std::string rcver) {
    if (this->raw_cmd.find(':') == std::string::npos) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOTEXTTOSEND(serverInstance->getHostname()));
        return ;
    }
    std::string msgPriv = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    if (rcver[0] == '#') {
        Channel *chan = serverInstance->getChannel(rcver);
        if (chan == NULL) {
            serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), senderClient->getNickName(), rcver));
            return ;
        }
        if (senderClient->getChannelsJoined().find(chan->getName()) == senderClient->getChannelsJoined().end()) {
            serverInstance->sendMsgToClient(senderClient->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), senderClient->getNickName(), chan->getName()));
            return ;
        }
        serverInstance->forwardMsgToChan(chan, senderClient->getNickName(), msgPriv, false);
        return ;
    }
    int recieverFd = serverInstance->getClientFd(rcver);
    if (recieverFd == 0) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOSUCHNICK(serverInstance->getHostname(), senderClient->getNickName(), rcver));
        return ;
    }
    std::cout << "sending privmsg ..... " << std::endl;
    serverInstance->sendMsgToClient(recieverFd, PRIVMSG_RPLY(senderClient->getNickName(), senderClient->getUserName(), senderClient->getIpAddr(), serverInstance->getClient(recieverFd)->getNickName(), msgPriv));
}

/* Channel naming:
    >> Apart from the requirement of the first character being a valid channel type prefix character;
        the only restriction on a channel name is that it may not contain any spaces (' ', 0x20),
        a control G / BELL ('^G', 0x07), or a comma (',', 0x2C) (which is used as a list item separator by the protocol).
*/
/*
    << JOIN #42chn
    >> :habuye_!~hab@5.195.225.158 JOIN #42chn
    >> :tngnet.nl.quakenet.org 332 habuye_ #42chn :Holla comasfdf
    >> :tngnet.nl.quakenet.org 333 habuye_ #42chn habex_ 1710078428
    >> :tngnet.nl.quakenet.org 353 habuye_ = #42chn :habuye_ habuye @habex_
    >> :tngnet.nl.quakenet.org 366 habuye_ #42chn :End of /NAMES list.
*/
void    Command::join(Client *client, Server *serverInstance, std::string chanName) {
    if (chanName[0] != '#' || chanName.find(7) != std::string::npos) {
        // incase of a channel name having a space, we can reconstruct the "name" but I am lazy now!
        //{:adrift.sg.quakenet.org 403 tesfa #fhfjk :No such channel}
        serverInstance->sendMsgToClient(client->getFd(), ERR_BADCHANMASK(serverInstance->getHostname(), client->getNickName()));
        return ;
    }
    Channel *chann = serverInstance->getChannel(chanName);
    if (chann == NULL)
    {
        serverInstance->createChannel(chanName, client, this);
        return ;
    }
    // we check if channel is an invite only:
    if (chann->isModeOn('i') && (chann->getAllInvitees().find(client->getNickName()) != chann->getAllInvitees().end())) {
        // check if the user has an invitation
        std::set<std::string>::iterator it = chann->getAllInvitees().find(client->getNickName());
        if (it == chann->getAllInvitees().end()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_INVITEONLYCHAN(serverInstance->getHostname(), client->getNickName(), chann->getName()));
            return ;
        }
        if (chann->isModeOn('l') && chann->getNumOfChanMembers() == chann->getUsersLimit()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_CHANNELISFULL(serverInstance->getHostname(), client->getNickName(), chann->getName()));
            return ;
        }
        // if not return;
        // if yes (one time use of invite -- delema)
    }
    /* before joining him to channel -- we have to check if he has a correct key and if the user limit of the channel is reached (both of them if nessesaary) */
    else if ((chann->isModeOn('k') || chann->isModeOn('l')))
    {
        if (chann->isModeOn('l') && chann->getNumOfChanMembers() == chann->getUsersLimit()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_CHANNELISFULL(serverInstance->getHostname(), client->getNickName(), chann->getName()));
            return ;
        }
        /* JOIN #42 */
        std::set<std::string>::iterator it = chann->getAllInvitees().find(client->getNickName());
        if (this->params.size() == 1 && it == chann->getAllInvitees().end()) {
            std::cout << " is me here\n";
            serverInstance->sendMsgToClient(client->getFd(), ERR_BADCHANNELKEY(serverInstance->getHostname(), client->getNickName(), chann->getName()));
            return ;
        }
        else if (it == chann->getAllInvitees().end() && this->params[1] != chann->getChanKey()){// he came without key or there is key mismatch
            std::cout << " is me here - 2\n";
            serverInstance->sendMsgToClient(client->getFd(), ERR_BADCHANNELKEY(serverInstance->getHostname(), client->getNickName(), chann->getName()));
            return ;
        }
        // at this point he got the correct key and channel user limmit hasnt been reached yet
    }
    if (chann->getAllMembersNick().find(client->getNickName()) != chann->getAllMembersNick().end()) {
        return ;
    }
    chann->getAllMembersNick().insert(client->getNickName());
    chann->insertToMemberFdMap(client->getNickName(), client->getFd());
    client->getChannelsJoined().insert(chann->getName());
    std::string letThemKnow = RPL_JOIN(client->getNickName(), client->getUserName(), client->getIpAddr(), chann->getName());
    serverInstance->forwardMsgToChan(chann, client->getNickName(), letThemKnow, true);
    // RPL_topic
    if (chann->getHasTopic() == true)
        serverInstance->sendMsgToClient(client->getFd(), RPL_TOPIC(serverInstance->getHostname(), client->getNickName(), chann->getName(), chann->getTopic()));
    // RPL_NAMES
    serverInstance->namesCmd(client, chanName);
    chann->getAllInvitees().erase(client->getNickName());
}

/*
    Command: KICK
    Parameters: <channel> <user> *( "," <user> ) [<comment>]

    recieved format: {KICK #42 tesfa_ :he is real bad man}
 */
void    Command::kick(Client *senderClient, Server *serverInstance, std::string potentialVictim) {
    // check if channel exists; if not, send error and return!
    Channel *chan = serverInstance->getChannel(this->params[0]);
    if (chan == NULL) {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), senderClient->getNickName(), this->params[0]));
        return ;
    }
    // check if "kicker" is a chanOp, if not send error and return
    std::string chanOp = chan->isClientaMember(senderClient->getNickName());
    if (chanOp == "") {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), senderClient->getNickName(), chan->getName()));
        return ;
    }
    if (chanOp[0] != '@') {
        // notice in your side only that sais you are not OP!
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getHostname(), senderClient->getNickName(), chan->getName()));
        return ;
    }
    // check if the user exists in said channel, if not send error and return
    std::string victim = chan->isClientaMember(potentialVictim);
    if (victim == "") {
        serverInstance->sendMsgToClient(senderClient->getFd(), ERR_USERNOTINCHANNEL(serverInstance->getHostname(), senderClient->getNickName(), chan->getName()));
        return ;
    }
    if (victim[0] == '@')
        victim = victim.substr(1);
    // remove him, send a message on why he is being kicked-out!
    // send a notice (with reason) that he is being kicked out from a channel
    std::string kickMsg = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    if (kickMsg == "")
        kickMsg = victim;
    std::string kickNotice = RPL_KICK(senderClient->getNickName(), senderClient->getUserName(), senderClient->getIpAddr(), chan->getName(), victim, kickMsg);
    // inform all user of the channel that someone got kicked out !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    /* 
        << KICK #lef tesfa____ :
        >> :tesfa___!~dd@5.195.225.158 KICK #lef tesfa____ :tesfa___
        >> :htg!~dd@5.195.225.158 KICK #ab42chan tesfa :bye bigman
        >> :habex!~hab@5.195.225.158 KICK #42chan tesfa :habex
     */
    serverInstance->forwardMsgToChan(chan, chanOp, kickNotice, true);
    serverInstance->removeClientFromChan(serverInstance->getClientByNick(victim), chan); 
}


void    Command::quit(Client *senderClient, Server *serverInstance) {
    std::string quitMsg = RPL_QUIT(senderClient->getNickName(), senderClient->getUserName(), senderClient->getIpAddr(), this->raw_cmd);
    serverInstance->removeClient(senderClient, quitMsg);
}

void    Command::part(Client *client, Server *serverInstance, std::string chanName) {
    /* :htg_!~dd@5.195.225.158 PART #gele */
    Channel *chan = serverInstance->getChannel(chanName);
    if (chan == NULL) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), client->getNickName(), chanName));
        return;
    }
    if (client->getChannelsJoined().find(chan->getName()) == client->getChannelsJoined().end()) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), client->getNickName(), chan->getName()));
        return ;
    }
    std::string reason = std::string("Default Leaving Reason");
    if (this->raw_cmd.find(':') != std::string::npos)
        reason = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    std::string partMsg = RPL_PART(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), reason);
    serverInstance->forwardMsgToChan(chan, client->getNickName(), partMsg, true);
    serverInstance->removeClientFromChan(client, chan);
}

void    Server::namesCmd(Client *client, std::string chanName) {
    Channel *channel = this->getChannel(chanName);
    if (channel == NULL) { // If channel doesn't exist, send error message to client
        this->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(this->getHostname(), client->getNickName(), chanName));
        return;
    }
    // Get the list of nicknames
    // the Op's first
    std::set<std::string> opNicks = channel->getAllChanOps();
    std::string namesReply = RPL_NAMES(this->getHostname(), client->getNickName(), chanName);
    std::set<std::string>::iterator it = opNicks.begin();
    // Construct the NAMES reply message
    for (; it != opNicks.end(); ++it) {
        namesReply += (*it) + " ";
    }
    // then the normal members
    std::set<std::string> nicknames = channel->getAllMembersNick(); // I USED reference (des kbleka ile tomas)
    std::set<std::string>::iterator it_n = nicknames.begin();
    for (; it_n != nicknames.end(); ++it_n) {
        if (channel->getAllChanOps().find("@" + *it_n) == channel->getAllChanOps().end())
            namesReply += (*it_n) + " ";
    }
    namesReply += "\r\n";
    // Send the NAMES reply message to the client
    this->sendMsgToClient(client->getFd(), namesReply);
    this->sendMsgToClient(client->getFd(), RPL_ENDOFNAMES(this->getHostname(), client->getNickName(), chanName));
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
    Channel *chan = serverInstance->getChannel(this->params[1]);
    if (chan == NULL) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), client->getNickName(), this->params[1]));
        return ;
    }
    // checking is invitor is in channel and is actuall a chanOp
    std::string chanOp = chan->isClientaMember(client->getNickName());
    if (chanOp == "") {
        serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), client->getNickName(), chan->getName()));
        return ;
    }
    if (chanOp[0] != '@') {
        serverInstance->sendMsgToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getHostname(), client->getNickName(), chan->getName()));
        return ;
    }
    // checking invitee's availability in server, if truee we check if he already in channel;
    Client *invitee = serverInstance->getClientByNick(this->params[0]);
    if (invitee == NULL) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHNICK(serverInstance->getHostname(), client->getNickName(), this->params[0]));
        return ;
    }
    if (invitee->getChannelsJoined().find(chan->getName()) != invitee->getChannelsJoined().end()) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_USERONCHANNEL(serverInstance->getHostname(), client->getNickName(), invitee->getNickName(), chan->getName()));
        return ;
    }
    // adding invitee's nick name in our record of invitee's (needed to be deleted after one time use)
    std::cout << "invited size: " << chan->getAllInvitees().size() << std::endl;
    chan->getAllInvitees().insert(invitee->getNickName());
    std::cout << "invited size: " << chan->getAllInvitees().size() << std::endl;
    serverInstance->sendMsgToClient(invitee->getFd(), RPL_YouIsInvited(client->getNickName(), client->getNickName(), client->getIpAddr(), chan->getName(), invitee->getNickName()));
    /* >> :afk!~hab@5.195.225.158 INVITE cfk #42chanasfaf */
}

/*
    The TOPIC command is used to change or view the topic of the given channel.
    If <topic> is not given, either RPL_TOPIC or RPL_NOTOPIC is returned specifying
    the current channel topic or lack of one. If <topic> is an empty string,
    the topic for the channel will be cleared.
*/
/* Format:
    Command: TOPIC
  Parameters: <channel> [<topic>]

    << TOPIC #42chn :this is new topic
    >> :habex_!~hab@5.195.225.158 TOPIC #42chn :this is new topic

    17:09 -!- Topic for #42chn: this is new topic
    17:09 -!- Topic set by habex_ [~hab@5.195.225.158] [Sun Mar 10 17:04:16 2024]
*/
void    Command::unsetTopic(Client *client, Server *serverInstance) {
    if (this->params.size() < 2) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(serverInstance->getHostname(), this->cmd));
        return ;
    }
    std::vector<std::string> channels = split(this->params[1], ',');
    for(std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); ++it) {
        Channel *chan = serverInstance->getChannel(*it);
        if (chan == NULL) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), client->getNickName(), *it));
            return ;
        }
        std::string chanOp = chan->isClientaMember(client->getNickName());
        if (chanOp == "") {
            serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), client->getNickName(), chan->getName()));
            return ;
        }
        chan->getTopic().erase();
        chan->getHasTopic() = false;
        serverInstance->forwardMsgToChan(chan, client->getNickName(), 
            TOPIC_CHANGE(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), chan->getTopic()), true);
    }
}

void    Command::topic(Client *client, Server *serverInstance, std::string chanName) {
    Channel *chan = serverInstance->getChannel(chanName);
    if (chan == NULL) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), client->getNickName(), chanName));
        return ;
    }
    std::string chanOp = chan->isClientaMember(client->getNickName());
    if (chanOp == "") {
        serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), client->getNickName(), chan->getName()));
        return ;
    }
    // all members can see topic of the channel
    //but to change it you have to be an OP
    if (this->params.size() == 1) {
        if (chan->getHasTopic())
            serverInstance->sendMsgToClient(client->getFd(), RPL_TOPIC(serverInstance->getHostname(), client->getNickName(), chan->getName(), chan->getTopic()));
        else 
            serverInstance->sendMsgToClient(client->getFd(), RPL_TOPICNOTSET(serverInstance->getHostname(), client->getNickName(), chan->getName()));
        return ;
    }
    if (chanOp[0] != '@') {
        serverInstance->sendMsgToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getHostname(), client->getNickName(), chan->getName()));
        return ;
    }
    std::string newTopic = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    chan->getTopic() = newTopic;
    chan->getHasTopic() = true;
    serverInstance->forwardMsgToChan(chan, client->getNickName(), 
        TOPIC_CHANGE(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), chan->getTopic()), true);
}


void    Command::notice(Client *client, Server *serverInstance, std::string reciever) {
    /* NOTICE_RPLY */
    if (this->raw_cmd.find(':') == std::string::npos) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOTEXTTOSEND(serverInstance->getHostname()));
        return ;
    }
    std::string msgPriv = this->raw_cmd.substr(this->raw_cmd.find(':') + 1);
    if (reciever[0] == '#') {
        Channel *chan = serverInstance->getChannel(reciever);
        if (chan == NULL) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), client->getNickName(), reciever));
            return ;
        }
        if (client->getChannelsJoined().find(chan->getName()) == client->getChannelsJoined().end()) {
            serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), client->getNickName(), chan->getName()));
            return ;
        }
        /* std::string(userHostMask(senderNick, senderUsername, clientIp)+" NOTICE "+revieverNick+" :"+msg+"\r\n") */
        std::string notice = NOTICE_RPLY(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), msgPriv);
        serverInstance->forwardMsgToChan(chan, client->getNickName(), notice, false);
        return ;
    }
    int recieverFd = serverInstance->getClientFd(reciever);
    if (recieverFd == 0) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHNICK(serverInstance->getHostname(), client->getNickName(), reciever));
        return ;
    }
    std::string notice = NOTICE_RPLY(client->getNickName(), client->getUserName(), client->getIpAddr(),  serverInstance->getClient(recieverFd)->getNickName(), msgPriv);
    serverInstance->sendMsgToClient(recieverFd, notice);
}
