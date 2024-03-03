/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel_modes.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 09:45:02 by hatesfam          #+#    #+#             */
/*   Updated: 2024/03/03 18:33:48 by hatesfam         ###   ########.fr       */
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
void Command::mode(Client *client, Server* serverInstance) {
    if (this->params[0][0] == '#') {
        if (this->params.size() == 1) {
            Channel *chan = serverInstance->getChanByName(this->params[0]);
            if (chan == NULL)
                return (serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHCHANNEL(serverInstance->getServerHostName(), client->getNickName(), chan->getChannelName())));
            std::string modesOn = "+";
            std::set<char>::iterator it = chan->getChannelModes().begin();
            for (; it != chan->getChannelModes().end(); ++it)
                modesOn += (*it);
            std::cout << "Modes On: " << modesOn << std::endl;
            /*
                << MODE #42chan
                >> :hostsailor.ro.quakenet.org 324 a__ #42chan +tnCN
                --> chanquery mode
                >> :hostsailor.ro.quakenet.org 329 a__ #42chan 1709438651
                --> event 329
            */
           serverInstance->sendMsgToClient(client->getFd(), RPL_CHANNELMODEIS(serverInstance->getServerHostName(), client->getNickName(), chan->getChannelName(), modesOn));
           std::cout << "channel mode query!\n";
           return ;
        }
        this->mode_channel(client, serverInstance);
    }
    else
        this->mode_user(client, serverInstance);
    
}

/* MODES - applied to channel */
/* Formats to expect: 
    1. MODE #Finnish +/-o Kilroy                    -- give/take operator privilege
    2. MODE #Finnish +/-i                           -- Set/remove Invite-only channel
    3. MODE #42 +k oulu / [MODE #42 -k ]            -- Set/remove the channel key (password)
    4. MODE #eu-opers +l 10 / [ MODE #eu-opers -l]  -- Set/remove the user limit to channel
*/
bool Command::mode_channel(Client *client, Server* serverInstance) {
    std::string chanName = this->params[0];
    Channel *chan = serverInstance->getChanByName(chanName);
    if (chan == NULL) {
        std::cout << "Channel DOESNT exist!\n";
        return false;
    }
    /* 
        << MODE #42chan -l
        >> :hostsailor.ro.quakenet.org 482 habex #42chan :You're not channel operator
        << MODE #42chan +l 7
        >> :hostsailor.ro.quakenet.org 482 habex #42chan :You're not channel operator
    */
    std::string chanOp = chan->isClientChanOp(client->getNickName());
    if (chanOp == "") {
        serverInstance->sendMsgToClient(client->getFd(), ERR_CHANOPRIVSNEEDED(serverInstance->getServerHostName(), client->getNickName(), chan->getChannelName()));
        return false;
    }
    // for now lets just consider our modes comes in (+m) or (-m) formats:
        // but in real irc channel, /mode <chan> +i-i+k... 
    // std::cout << "\n---------------------------------------\n";
    // std::vector<std::string> modes = split(this->params[1], '-', '+');
    // std::vector<std::string>::iterator it = modes.begin();
    // for (; it != modes.end(); ++it) {
    //     if (it != modes.begin())
    //         std::cout << ", ";
    //     std::cout << (*it);
    // }
    // std::cout << "\n---------------------------------------\n";
    if (this->params[1] == "+i" || this->params[1] == "-i")
       return (this->mode_i(chan, client, serverInstance));
    if (this->params[1] == "+o" || this->params[1] == "-o")
       return (this->mode_o(chan, client, serverInstance));
    if (this->params[1] == "+k" || this->params[1] == "-k")
       return (this->mode_k(chan, client, serverInstance));
    if (this->params[1] == "+l" || this->params[1] == "-l")
       return (this->mode_l(chan, client, serverInstance));
    // if its other than those, we will say something and return
    return false;
}


// channel modes
bool    Command::mode_o(Channel *chan, Client *client, Server* serverInstance) {
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
    std::string groom = chan->isClientaMember(this->params[2]);
    if (groom == "") // if he is not in channel
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_NOSUCHNICK(serverInstance->getServerHostName(), client->getNickName(), this->params[2])), true);
    if (this->params[1] == "+o") {
        if (chan->isClientChanOp(groom) != "") // if he is a channelOp already we just ignore
            return true;
        std::string modeRply = RPL_OPERATORGIVEREVOKE(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getChannelName(), "+o", this->params[2]);
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
        chan->getAllChanOps().insert("@" + client->getNickName());
        return true;
    }
    if (this->params[1] == "-o") {
        if (chan->isClientChanOp(groom) == "") // if he is a groom is not OP, we just ignore
        {
            std::cout << "===> exiting outta this!!!\n";
            return true;
        }
            std::cout << "===> exiting outta this!!!\n";
        std::string modeRply = RPL_OPERATORGIVEREVOKE(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getChannelName(), "-o", this->params[2]);
            std::cout << "===> exiting outta this!!!\n";
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
            std::cout << "===> exiting outta this!!!\n";
        chan->getAllChanOps().erase("@" + client->getNickName());
            std::cout << "===> exiting outta this!!!\n";
    }
    return false;
}
bool    Command::mode_i(Channel *chan, Client *client, Server* serverInstance) {
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
        if (chan->isModeOn('l') == false) {
            return (std::cout << "the key wasnt set!\n", false);
        }
        // we will see if any error handling is needed!
        chan->getChannelModes().erase('l');
        // let the group know that setting has changed!
        std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getChannelName(), std::string("-l"));
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
        return (chan->setUsersLimit(0), true);
    }
    // setting +l
    if (this->params.size() != 3)
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(serverInstance->getServerHostName(), this->cmd)), false);
    int num = atoi(this->params[2].c_str()); // we will have to check if it contains anythiing thats now a digit and return an error
    if (num <= 0 || num > INT_MAX) // if userlimit entered is 0, more than int max or some invalid value, we just ignore
        return (false);
    // check if the num is less than zero before setting it!
    chan->getChannelModes().insert('l'); // adding 'l' to indicate that we have the userlimit set for the channel
    std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getChannelName(), std::string("+l " + this->params[2]));
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
        std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getChannelName(), std::string("-k " + chan->getChanKey()));
        serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
        return (chan->setChanKey(""), true);
    }
    // setting +k
    /* 
        << MODE #habexirc +k ollata
        >> :hostsailor.ro.quakenet.org 467 gaim #habexirc :Channel key already set
    */
    if (this->params.size() != 3)
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(serverInstance->getServerHostName(), this->cmd)), false);
    if (chan->isModeOn('k') == true) // if we have key set alrady
        return (serverInstance->sendMsgToClient(client->getFd(), ERR_CHANKEYALREADYSET(serverInstance->getServerHostName(), client->getUserName(), chan->getChannelName())), false);
    chan->getChannelModes().insert('k');
    if (this->params[2].size() > 23) // overflow protection
        chan->setChanKey(this->params[2].substr(0, 23)); // this is how irssi does it
    else
        chan->setChanKey(this->params[2]);
    std::string modeRply = RPL_MODES(client->getNickName(), client->getUserName(), client->getIpAddr(), chan->getChannelName(), std::string("+k " + chan->getChanKey()));
    serverInstance->sendMessageToChan(chan, client->getNickName(), modeRply, true);
    return (true);
}

/* MODES - applied to users */
void Command::mode_user(Client *client, Server* serverInstance) {
}