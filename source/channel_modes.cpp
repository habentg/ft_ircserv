/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel_modes.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 09:45:02 by hatesfam          #+#    #+#             */
/*   Updated: 2024/03/10 20:51:46 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

/* 
    ◦ Then, you have to implement the commands that are specific to channel
        operators:
        ∗ INVITE - Invite a client to a channel
        ∗ MODE - Change the channel’s mode:
            · i: Set/remove Invite-only channel
            · t: Set/remove the restrictions of the TOPIC command to channel operators
            · k: Set/remove the channel key (password)
            · o: Give/take channel operator privilege
            · l: Set/remove the user limit to channel
*/
/* command format to expect: -> [ MODE <target> <+/-mode> ... ] */
bool Command::mode(Client *client, Server* serverInstance) {
    // 
    if (this->params[0][0] == '#') {
        Channel *chan = serverInstance->getChanByName(this->params[0]);
        if (chan == NULL)
            return (serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getHostname(), client->getNickName(), this->params[0])), false);
        std::string sender = chan->isClientaMember(client->getNickName());
        if (sender == "")
            return (serverInstance->sendMsgToClient(client->getFd(), ERR_YouIsNotInCHANNEL(serverInstance->getHostname(), client->getNickName(), chan->getName())), false);
        if (this->params.size() == 1) {
            std::string modesOn = "+";
            std::set<char>::iterator it = chan->getChannelModes().begin();
            for (; it != chan->getChannelModes().end(); ++it)
                modesOn += (*it);
            serverInstance->sendMsgToClient(client->getFd(), RPL_CHANNELMODEIS(serverInstance->getHostname(), client->getNickName(), chan->getName(), modesOn));
            return true;
        }
        if (sender[0] != '@')
            return (serverInstance->sendMsgToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getHostname(), client->getNickName(), chan->getName())), false);
        this->mode_channel(client, chan, serverInstance);
    }
    else
        this->mode_user(client, serverInstance);
    return true;
}

/* MODES - applied to channel */
/* Formats to expect: 
    1. MODE #Finnish +/-o Kilroy                    -- give/take operator privilege
    2. MODE #Finnish +/-i                           -- Set/remove Invite-only channel
    3. MODE #42 +k oulu / [MODE #42 -k ]            -- Set/remove the channel key (password)
    4. MODE #eu-opers +l 10 / [ MODE #eu-opers -l]  -- Set/remove the user limit to channel
*/
bool Command::mode_channel(Client *client, Channel *chan, Server* serverInstance) {
    // for now lets just consider our modes comes in (+m) or (-m) formats:
        // but in real irc channel, /mode <chan> +i-i+k... 
    // std::cout << "\n---------------------------------------\n";
    // std::vector<std::string> modes = split(this->params[1], '-');
    // std::vector<std::string>::iterator it = modes.begin();
    // for (; it != modes.end(); ++it) {
    //     if (it != modes.begin())
    //         std::cout << ", ";
    //     std::cout << (*it);
    // }
    // std::cout << "\n---------------------------------------\n";
    if (this->params[1] == "i" || this->params[1] == "+i" || this->params[1] == "-i")
       return (this->mode_i(chan, client, serverInstance));
    if (this->params[1] == "o" || this->params[1] == "+o" || this->params[1] == "-o")
       return (this->mode_o(chan, client, serverInstance));
    if (this->params[1] == "k" || this->params[1] == "+k" || this->params[1] == "-k")
       return (this->mode_k(chan, client, serverInstance));
    if (this->params[1] == "l" || this->params[1] == "+l" || this->params[1] == "-l")
       return (this->mode_l(chan, client, serverInstance));
    if (this->params[1] == "t" || this->params[1] == "+t" || this->params[1] == "-t")
       return (this->mode_t(chan, client, serverInstance));
    serverInstance->sendMsgToClient(client->getFd(), ERR_UNKNOWNMODECHAR(serverInstance->getHostname(), client->getNickName(), "F"));
    return false;
}


// channel modes
    /* MODE #channel +o nick */
    /* 
        << MODE #habexirc +o tesfa
        >> :gaim!~dd@5.195.225.158 MODE #habexirc +o tesfa
        << MODE #habexirc -o tesfa
        >> :gaim!~dd@5.195.225.158 MODE #habexirc -o tesfa
        << MODE #habexirc -o tesfa
        << MODE #habexirc -o tesfa
        << MODE #habexirc -o tesfa
        << MODE #habexirc -o tesfa
        << MODE #habexirc +oasdfhashfahf tesfa
        >> :hostsailor.ro.quakenet.org 472 gaim a :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim d :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim f :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim h :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim a :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim h :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim f :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim a :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim h :is unknown mode char to me
        >> :hostsailor.ro.quakenet.org 472 gaim f :is unknown mode char to me
        >> :gaim!~dd@5.195.225.158 MODE #habexirc +so tesfa
    */
bool    Command::mode_o(Channel *chan, Client *client, Server* serverInstance) {
    std::string groom = chan->isClientaMember(this->params[2]);
    if (groom == "") // if he is not in channel
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHNICK(serverInstance->getHostname(), client->getNickName(), this->params[2])), true);
    if (this->params[1] == "o" || this->params[1] == "+o") {
        if (groom[0] == '@') // if he is a channelOp already we just ignore
            return true;
        std::string modeRply = RPL_OPERATORGIVEREVOKE(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), "+o", this->params[2]);
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
        chan->getAllChanOps().insert("@" + groom);
        return true;
    }
    //settting -o
    if (groom[0] != '@') // if he is a groom is not OP, we just ignore
        return true;
    std::string modeRply = RPL_OPERATORGIVEREVOKE(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), "-o", this->params[2]);
    serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
    chan->getAllChanOps().erase("@" + groom);
    return false;
}
bool    Command::mode_i(Channel *chan, Client *client, Server* serverInstance) {
    if (this->params[1] == "i" || this->params[1] == "+i") {
        // check if channel is alrady an "invite-only" channel
        if (chan->isModeOn('i'))
            return true;
        // let them know that we exclusive!!!!!!
        chan->getChannelModes().insert('i');
        std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), std::string("+i"));
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
    }
    // setting -i
    if (chan->isModeOn('i') == false)
        return false;
    // let them know that we aint exclusive anymore!!!!!
    chan->getChannelModes().erase('i');
    std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), std::string("-i"));
    serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
    return false;
}
/* Format:
    << MODE #42chan +l 3
    >> :a__!~dd@5.195.225.158 MODE #42chan +l 3
    << MODE #42chan -l
    >> :a__!~dd@5.195.225.158 MODE #42chan -l
    >> :gaim!~dd@5.195.225.158 MODE #habexirc +l 3
    << MODE #habexirc +l 3f3
    << MODE #habexirc +l a7fdasfasfadfasf
    << MODE #habexirc +l af7fdasfasfadfasf
        >> no response
*/
        /* if there is no 'l' mode already set at our channel, we just ignore the '-l' request*/
        /*
            << MODE #42chan -l
            << MODE #42chan -l
            << MODE #42chan -l
        */
bool    Command::mode_l(Channel *chan, Client *client, Server* serverInstance) {
    if (this->params[1] == "-l") {
        if (chan->isModeOn('l') == false) { // if limit wasnot set ... just ignore
            return (false);
        }
        // we will see if any error handling is needed!
        chan->getChannelModes().erase('l');
        // let the group know that setting has changed!
        std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), std::string("-l"));
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
        return (chan->setUsersLimit(0), true);
    }
    // setting +l/l
    if (this->params.size() != 3)
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(serverInstance->getHostname(), this->cmd)), false);
    int num = atoi(this->params[2].c_str()); // we will have to check if it contains anythiing thats now a digit and return an error
    if (num <= 0 || num > INT_MAX) // if userlimit entered is 0, more than int max or some invalid value, we just ignore
        return (false);
    // check if the num is less than zero before setting it!
    chan->getChannelModes().insert('l'); // adding 'l' to indicate that we have the userlimit set for the channel
    std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), std::string("+l " + this->params[2]));
    serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
    /* we will twik the joining a channel feature to check for user limit before a client joins */
    return (chan->setUsersLimit(num), true);
}

/* the reak irc is behaving in a wierd way when unsetting the key multiple times when it wasnt set */
bool    Command::mode_k(Channel *chan, Client *client, Server* serverInstance) {
    if (this->params[1] == "-k") {
        if (chan->isModeOn('k') == false) { 
            return (std::cout << "the key wasnt set!\n", false);
        }
        chan->getChannelModes().erase('k');
        std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), std::string("-k " + chan->getChanKey()));
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
        return (chan->setChanKey(""), true);
    }
    // setting +k/k
    /* 
        << MODE #habexirc +k ollata
        >> :hostsailor.ro.quakenet.org 467 gaim #habexirc :Channel key already set
    */
    if (this->params.size() != 3)
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(serverInstance->getHostname(), this->cmd)), false);
    if (chan->isModeOn('k') == true) // if we have key set alrady
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_CHANKEYALREADYSET(serverInstance->getHostname(), client->getUserName(), chan->getName())), false);
    chan->getChannelModes().insert('k');
    if (this->params[2].size() > 23) // overflow protection
        chan->setChanKey(this->params[2].substr(0, 23)); // this is how irssi does it
    else
        chan->setChanKey(this->params[2]);
    std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getName(), std::string("+k " + chan->getChanKey()));
    serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
    return (true);
}

bool    Command::mode_t(Channel *chan, Client *client, Server* serverInstance) {
    if (this->params[1] == "-t") {
        if (chan->isModeOn('t') == false)
            return (std::cout << "chan is not 't' \n", false);
    }
    std::cout << "we in +t\n";
}
/* MODES - applied to users */
void Command::mode_user(Client *client, Server* serverInstance) {
}