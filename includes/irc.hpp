/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:41:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/19 16:19:26 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

# include <iostream>
# include <exception>
# include <string>
# include <cstdlib>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <poll.h>
# include <signal.h>
# include <vector>
# include <map>


#define CAP_LS_RESP(client_ip)          std::string(":" + client_ip + " CAP * LS :multi-prefix\r\n")
#define CAP_ACK_RESP(client_ip)         std::string(":" + client_ip + " CAP * ACK :multi-prefix\r\n")

#define RPL_WELCOME(client_ip, hostname, username, nickname) std::string(":" + client_ip + " 001 " + nickname + \
    " :Welcome to the Internet Relay Network, " + nickname + "!" + username + "@" + hostname + "\r\n")
#define RPL_YOURHOST(client_ip, hostname)          std::string(":" + client_ip + " 002 " + client_ip + \
    " :Your host is " + hostname + ", running version <1.0>\r\n")
#define RPL_CREATED(client_ip, hostname)           std::string(":" + client_ip + " 003 " + client_ip + \
    " :" + hostname +" was created <human-readable date/time>\r\n")
#define RPL_MYINFO(client_ip, hostname)            std::string(":" + client_ip + " 004 " + client_ip + \
    " :CLIENT SHOULD DESCOVER AVAILABLE FEATURES using RPL_ISUPPORT tokens <NO FUCKING IDEA HOW TO DO THIS ... TOMORROWS PROBLEM!>\r\n")
#define RPL_ISUPPORT(client_ip)          std::string(":" + client_ip + " 005 " + client_ip + \
    " :are supported by this server <tomorrows problem>\r\n")

#define RPL_MOTDSTART(client_ip, hostname)         std::string(":" + client_ip + " 375 " + client_ip + \
    " :- " + hostname + " Message of the day - \r\n")
#define RPL_MOTD(client_ip)              std::string(":" + client_ip + " 372 " + client_ip + \
    " :- MY NAME IS ...... \r\n")
#define RPL_ENDOFMOTD(client_ip, hostname)         std::string(":" + client_ip + " 376 " + client_ip + \
    " :" +hostname+ " End of /MOTD command.\r\n")


#define GOODBYE(client_ip, nickname)    std::string(": " + client_ip + " 451 :Nickname already in use\r\n")

# include "client.hpp"
# include "server.hpp"
# include "error.hpp"
# include "channel.hpp"

std::vector<std::string> split(std::string& str, char delimiter);
std::vector<std::string> newline_split(std::string& str);

#endif // !IRC_HPP