/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/21 12:42:05 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/22 13:23:17 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
# define COMMAND_HPP

# include "irc.hpp"

class Client;

class Command {
    public:
        std::string cmd;
        std::vector<std::string> params;
        std::string raw_cmd;
        Command(std::string rcved_cmd);
        ~Command(void);
        /* COMMANDS */
        void    password(Client *client, Server* serverInstance);
        bool    nickname(Client *client, Server* serverInstance);
        void    user(Client *client, Server* serverInstance);
        void    privmsg(Client *senderClient, Server *serverInstance);
        void    join(Client *senderClient, Server *serverInstance);
        void    kick(Client *senderClient, Server *serverInstance);
        void    quit(Client *senderClient, Server *serverInstance);
        void    partLeavChan(Client *senderClient, Server *serverInstance);
        void    names(Client *client, Server *serverInstance);
        bool    mode(Client *client, Server *serverInstance);
        void    invite(Client *client, Server *serverInstance);
        void    topic(Client *client, Server *serverInstance);
        bool    mode_channel(Client *client, Channel *chan, Server* serverInstance);
        void    mode_user(Client *client, Server* serverInstance);
        bool    mode_i(Channel *chan, Client *client, Server* serverInstance);
        bool    mode_o(Channel *chan, Client *client, Server* serverInstance);
        bool    mode_k(Channel *chan, Client *client, Server* serverInstance);
        bool    mode_l(Channel *chan, Client *client, Server* serverInstance);
        bool    mode_t(Channel *chan, Client *client, Server* serverInstance);
};

size_t validNickName(std::vector<std::string> nick_params, int clientFd, Server* serverInstance, std::string cmdName);

#endif //!COMMAND_HPP