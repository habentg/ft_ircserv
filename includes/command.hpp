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
        void    password(Client *client, Server* servInstance);
        bool    nickname(Client *client, Server* servInstance);
        void    user(Client *client, Server* servInstance);
};


bool validNickName(std::vector<std::string> nick_params, int clientFd, Server* servInstance, std::string cmdName);

#endif //!COMMAND_HPP