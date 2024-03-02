/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   channel_modes.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/02 09:45:02 by hatesfam          #+#    #+#             */
/*   Updated: 2024/03/02 15:04:42 by hatesfam         ###   ########.fr       */
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
    if (this->params.size() < 2) {
        serverInstance->sendMsgToClient(client->getFd(), ERR_NEEDMOREPARAMS(serverInstance->getServerHostName(), std::string("NAMES")));
        return ;
    }
    /* will do any error handling here before proceeding to apply modes based on who is the target */
    if (this->params[0][0] == '#') 
        this->mode_channel(client, serverInstance);
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
void Command::mode_channel(Client *client, Server* serverInstance) {
}

/* MODES - applied to users */
void Command::mode_user(Client *client, Server* serverInstance) {
}
