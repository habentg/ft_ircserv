/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:41:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/24 02:06:35 by hatesfam         ###   ########.fr       */
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
# include <set>

/*  */
# include "client.hpp"
# include "server.hpp"
# include "channel.hpp"
# include "command.hpp"

/*  */
#define CAP_LS_RESP(hostname)          std::string(":"+hostname+" CAP * LS :multi-prefix\r\n")
#define CAP_ACK_RESP(hostname)         std::string(":"+hostname+" CAP * ACK :multi-prefix\r\n")

/*  */
#define RPL_WELCOME(hostname, username, nickname)    std::string(":"+hostname+" 001 "+nickname+" :Welcome to the Internet Relay Network, "+nickname+"!"+username+"@"+hostname+"\r\n")
#define RPL_YOURHOST(hostname, nickname)             std::string(":"+hostname+" 002 "+nickname+" :Your host is "+hostname+", running version <1.0>\r\n")
#define RPL_CREATED(hostname, nickname)              std::string(":"+hostname+" 003 "+nickname+" :"+hostname +" was created <human-readable date/time>\r\n")
#define RPL_MYINFO(hostname, nickname)               std::string(":"+hostname+" 004 "+nickname+" :CLIENT SHOULD DESCOVER AVAILABLE FEATURES using RPL_ISUPPORT tokens <NO FUCKING IDEA HOW TO DO THIS ... TOMORROWS PROBLEM!>\r\n")
#define RPL_ISUPPORT(hostname, nickname)             std::string(":"+hostname+" 005 "+nickname+" USERLEN=15: are supported by this server\r\n")
#define USERLEN 15 // we will see how to advertise this to the clients

/* MOTD */
#define RPL_MOTDSTART(hostname, nickname)            std::string(":"+hostname+" 375 "+nickname+" :- "+hostname+" Message of the day - \r\n")
#define RPL_MOTD(hostname, nickname)                 std::string(":"+hostname+" 372 "+nickname+" :- MY NAME IS ...... \r\n")
#define RPL_ENDOFMOTD(hostname, nickname)            std::string(":"+hostname+" 376 "+nickname+" :" +hostname+ " End of /MOTD command.\r\n")

/* PONG */
#define PONG(hostname)                              std::string("PONG :"+hostname +"\r\n")

/* Server to Client Error Responses */
#define ERR_NEEDMOREPARAMS(hostname, cmd)                           std::string(":"+hostname+" 464 "+cmd+" :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(hostname)                             std::string(":"+hostname+" 462 :You are already registered\r\n")
#define ERR_PASSWDMISMATCH(hostname)                                std::string(":"+hostname+" 464 :Password incorrect\r\n")
#define ERR_NICKNAMEINUSE(hostname, nickname)                       std::string(":"+hostname+" 433 * "+nickname +" :Nickname already in use\r\n")
#define ERR_ERRONEUSNICKNAME(hostname)                              std::string(":"+hostname+" 432 :you have characters in your nickname that are not supported in our server\r\n")
#define ERR_NONICKNAMEGIVEN(hostname)                               std::string(":"+hostname+" 431 :you havent provided with a nickname yet\r\n")
#define ERR_NOSUCHNICK(hostname, nickname, rcvNick)                          std::string(":"+hostname+" 401 "+nickname+rcvNick+" :No such user in our server\r\n")
#define ERR_NOTEXTTOSEND(hostname)                                  std::string(":"+hostname+" 412 :No text to send\r\n")
#define ERR_REGISTER_FIRST(hostname)                                std::string(":"+hostname+" 451 * :Register first\r\n")
#define ERR_NOPRIVILEGES(hostname, nickname)                        std::string(":"+hostname+" 481 "+nickname+" :Permission to KILL a user DENAID, You aint an Server-OP!\r\n")
#define ERR_BADCHANNELKEY(hostname, nickname)                       std::string(":"+hostname+" 475 "+nickname+" :channel key mismatch\r\n")
#define ERR_BADCHANMASK(hostname, nickname)                         std::string(":"+hostname+" 476 "+nickname+" :invalid channel name\r\n")
#define ERR_CHANOPRIVSNEEDED(hostname, nickname, chanName)                    std::string(":"+hostname+" 482 "+nickname+chanName+" :you are not channel OP\r\n")
#define ERR_NOSUCHCHANNEL(hostname, nickname, chanName)                       std::string(":"+hostname+" 403 "+nickname+chanName+" :no such channel\r\n")
#define ERR_YouIsNotInCHANNEL(hostname, nickname, chanName)         std::string(":"+hostname+" 442 "+nickname+chanName+" :you are not in channel, so you cant send\r\n")
#define ERR_USERNOTINCHANNEL(hostname, nickname, chanName)         std::string(":"+hostname+" 441 "+nickname+chanName+" :user is is not in channel\r\n")

/* REPLAY */
#define userHostMask(senderNick, senderUsername, hostname)                      std::string(":"+senderNick+"!"+senderUsername+"@"+hostname)
#define PRIVMSG_RPLY(senderNick, senderUsername, hostname, revieverNick, msg)   std::string(userHostMask(senderNick, senderUsername, hostname)+" PRIVMSG "+revieverNick+" :"+msg+"\r\n")
#define JOIN_RPLY(senderNick, senderUsername, hostname, chanName)               std::string(userHostMask(senderNick, senderUsername, hostname)+" JOIN "+chanName)

/* Error responses */
# define INCORRECT_ARGS "Error: Incorrect Numner of arguments!!\n    Usage: ./ft_ircserv <port number <password>"
# define INVALID_serverPort "Invalid port number. Please use a port number between 1024 and 65535."
# define PASSWORD_TOO_SHORT "\nError: Password to short or too long!!\n\n    Password should be at least 4 and 15 characters at most\n"


/* Command struct/class 
    -> Generally [COMMAND <parameters>] is gonna be the format of the commands
*/




unsigned short int          validate_input(int ac, char **av);
std::vector<std::string>    split(std::string& str, char delimiter);
void                        printVector(std::vector<std::string> vec);
std::string                 lowerCaseString(std::string str);

#endif // !IRC_HPP