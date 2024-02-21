/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:41:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/21 15:18:29 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

/*  */
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

/*  */
# include "client.hpp"
# include "server.hpp"
# include "channel.hpp"
# include "command.hpp"

/*  */
#define CAP_LS_RESP(client_ip)          std::string(":" + client_ip + " CAP * LS :multi-prefix\r\n")
#define CAP_ACK_RESP(client_ip)         std::string(":" + client_ip + " CAP * ACK :multi-prefix\r\n")

/*  */
#define RPL_WELCOME(client_ip, hostname, username, nickname)    std::string(":" + client_ip + " 001 " + nickname + " :Welcome to the Internet Relay Network, " + nickname + "!" + username + "@" + hostname + "\r\n")
#define RPL_YOURHOST(client_ip, hostname)                       std::string(":" + client_ip + " 002 " + client_ip + " :Your host is " + hostname + ", running version <1.0>\r\n")
#define RPL_CREATED(client_ip, hostname)                        std::string(":" + client_ip + " 003 " + client_ip + " :" + hostname +" was created <human-readable date/time>\r\n")
#define RPL_MYINFO(client_ip, hostname)                         std::string(":" + client_ip + " 004 " + client_ip + " :CLIENT SHOULD DESCOVER AVAILABLE FEATURES using RPL_ISUPPORT tokens <NO FUCKING IDEA HOW TO DO THIS ... TOMORROWS PROBLEM!>\r\n")
#define RPL_ISUPPORT(client_ip)                                 std::string(":" + client_ip + " 005 " + client_ip + " :are supported by this server <tomorrows problem>\r\n")

/* MOTD */
#define RPL_MOTDSTART(client_ip, hostname)                      std::string(":" + client_ip + " 375 " + client_ip + " :- " + hostname + " Message of the day - \r\n")
#define RPL_MOTD(client_ip)                                     std::string(":" + client_ip + " 372 " + client_ip + " :- MY NAME IS ...... \r\n")
#define RPL_ENDOFMOTD(client_ip, hostname)                      std::string(":" + client_ip + " 376 " + client_ip + " :" +hostname+ " End of /MOTD command.\r\n")

/* Server to Client Error Responses */
#define GOODBYE(client_ip, nickname)    std::string(":" + client_ip + " 451 :Nickname already in use\r\n")
#define WRONG_PASS(client_ip)           std::string(":" + client_ip + " 464 :Password mismatch\r\n")


/* Error responses */
# define INCORRECT_ARGS "Error: Incorrect Numner of arguments!!\n    Usage: ./ft_ircserv <port number <password>"
# define INVALID_PORT_NUMBER "Invalid port number. Please use a port number between 1024 and 65535."
# define PASSWORD_TOO_SHORT "\nError: Password to short!!\n\n    Password should be at least 4 characters\n"


/* Command struct/class 
    -> Generally [COMMAND <parameters>] is gonna be the format of the commands
*/




double validate_input(int ac, char **av);
std::vector<std::string> split(std::string& str, char delimiter);
std::vector<std::string> newline_split(std::string& str);
void printVector(std::vector<std::string> vec);

#endif // !IRC_HPP