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
# include <ctime>
# include <climits>
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
# include <fstream>

/*  */
# include "client.hpp"
# include "server.hpp"
# include "channel.hpp"
# include "command.hpp"

/*  */
#define CAP_LS_RESP(hostname)          std::string(":"+hostname+" CAP * LS :multi-prefix\r\n")
#define CAP_ACK_RESP(hostname)         std::string(":"+hostname+" CAP * ACK :multi-prefix\r\n")

/* Connection Closing */
#define Conn_closed(hostname)          std::string(":"+hostname+" Connection Closed By the Server\r\n")

/*  */
#define RPL_WELCOME(hostname, username, nickname)    std::string(":"+hostname+" 001 "+nickname+" :Welcome to the Internet Relay Network, "+nickname+"!"+username+"@"+hostname+"\r\n")
#define RPL_YOURHOST(hostname, nickname)             std::string(":"+hostname+" 002 "+nickname+" :Your host is "+hostname+", running version <1.0>\r\n")
#define RPL_CREATED(hostname, nickname, datetime)    std::string(":"+hostname+" 003 "+nickname+" :This server was created on <"+datetime+">\r\n")
#define RPL_MYINFO(hostname, nickname)               std::string(":"+hostname+" 004 "+nickname+" "+hostname+"\r\n")
#define RPL_ISUPPORT(hostname, nickname)             std::string(":"+hostname+" 005 "+nickname+" NICKLEN=15 MAXNICKLEN=15 TOPICLEN=250 CHANNELLEN=200 MAXCHANNELLEN=200 CHANTYPES=# CHANMODES=k,l,i,t,o CASEMAPPING=rfc1459 NETWORK=QuakeNet: are supported by this server\r\n")
#define USERLEN 15 // we will see how to advertise this to the clients

/* MOTD */
#define RPL_MOTDSTART(hostname, nickname)            std::string(":"+hostname+" 375 "+nickname+" :- "+hostname+" Message of the day - \r\n")
#define RPL_MOTD(hostname, nickname, msg)                 std::string(":"+hostname+" 372 "+nickname+" :- "+msg+"\r\n")
#define RPL_ENDOFMOTD(hostname, nickname)            std::string(":"+hostname+" 376 "+nickname+" :" +hostname+ " End of /MOTD command.\r\n")

/* PONG */
#define PONG(hostname)                              std::string("PONG :"+hostname +"\r\n")

/* Server to Client Error Responses */
#define ERR_NEEDMOREPARAMS(hostname, cmd)                           std::string(":"+hostname+" 464 "+cmd+" :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(hostname)                             std::string(":"+hostname+" 462 :You are already authenticated brodi\r\n")
#define ERR_PASSWDMISMATCH(hostname)                                std::string(":"+hostname+" 464 :Password incorrect\r\n")
#define ERR_NICKNAMEINUSE(hostname, nickname)                       std::string(":"+hostname+" 433 * "+nickname +" :Nickname already in use\r\n")
#define ERR_ERRONEUSNICKNAME(hostname)                              std::string(":"+hostname+" 432 :you have characters in your nickname that are not supported in our server\r\n")
#define ERR_NONICKNAMEGIVEN(hostname)                               std::string(":"+hostname+" 431 :you havent provided with a nickname yet\r\n")
/*  
    << MODE #habexirc +o tesfazgi
    >> :hostsailor.ro.quakenet.org 401 gaim tesfazgi :No such nick
*/
/* >> :adrift.sg.quakenet.org 401 kk zeyelepeo :No such nick */
#define ERR_NOSUCHNICK(hostname, nickname, rcvNick)                          std::string(":"+hostname+" 401 "+nickname+" "+rcvNick+" :No such user\r\n")
#define ERR_NOTEXTTOSEND(hostname)                                  std::string(":"+hostname+" 412 :No text to send\r\n")
#define ERR_REGISTER_FIRST(hostname)                                std::string(":"+hostname+" 451 * :Register first\r\n")
#define ERR_NOPRIVILEGES(hostname, nickname)                        std::string(":"+hostname+" 481 "+nickname+" :Permission to KILL a user DENAID, You aint an Server-OP!\r\n")
/* >> :hostsailor.ro.quakenet.org 471 a__ #habexirc :Cannot join channel, Channel is full (+l) */
#define ERR_CHANNELISFULL(hostname, nickname, chanName)             std::string(":"+hostname+" 471 "+nickname+" "+chanName+" :channel is FULL \r\n")
/* >> :hostsailor.ro.quakenet.org 475 gaim #new42chan :Cannot join channel, you need the correct key (+k) */
#define ERR_BADCHANNELKEY(hostname, nickname, chanName)             std::string(":"+hostname+" 475 "+nickname+" "+chanName+" :bad channel key\r\n")
#define ERR_BADCHANMASK(hostname, nickname)                         std::string(":"+hostname+" 476 "+nickname+" :invalid channel name\r\n")
#define ERR_CHANOPRIVSNEEDED(hostname, nickname, chanName)                    std::string(":"+hostname+" 482 "+nickname+" "+chanName+" :you are not channel OP\r\n")
#define ERR_NOSUCHCHANNEL(hostname, nickname, chanName)                       std::string(":"+hostname+" 403 "+nickname+" "+chanName+" :no such channel\r\n")
#define ERR_YouIsNotInCHANNEL(hostname, nickname, chanName)         std::string(":"+hostname+" 442 "+nickname+" "+chanName+" :you are not in channel, so you cant send\r\n")
#define ERR_USERNOTINCHANNEL(hostname, nickname, chanName)         std::string(":"+hostname+" 441 "+nickname+" "+chanName+" :user is not in channel\r\n")
#define ERR_CHANKEYALREADYSET(hostname, nickname, chanName)             std::string(":"+hostname+" 467 "+nickname+" "+chanName+" :Channel key is already set (+k)\r\n")
/*  << JOIN #42chan
    >> :stockholm.se.quakenet.org 473 bfk #42chan :Cannot join channel, you must be invited (+i)
*/
#define ERR_INVITEONLYCHAN(hostname, nick, chanName)                std::string(":"+hostname+" 473 "+nick+" "+chanName+" :channel is invite only, you cant join\r\n")
/* >> :atw.hu.quakenet.org 472 habex_ f :is unknown mode char to me */
#define ERR_UNKNOWNMODECHAR(hostname, nick, char)                   std::string(":"+hostname+" 472 "+nick+" "+char+" :is unknown mode character\r\n")
/* 
    << INVITE zfk #42chan
    >> :underworld2.no.quakenet.org 443 afk zfk #42chan :is already on channel
*/
#define ERR_USERONCHANNEL(hostname, nick, inviteeNick, chanName)                std::string(":"+hostname+" 443 "+nick+" "+inviteeNick+" "+chanName+" :user is already in channel\r\n")

/* REPLAY */
#define userHostMask(senderNick, senderUsername, clientIp)                                   std::string(":"+senderNick+"!"+senderUsername+"@"+clientIp)
#define PRIVMSG_RPLY(senderNick, senderUsername, clientIp, revieverNick, msg)                std::string(userHostMask(senderNick, senderUsername, clientIp)+" PRIVMSG "+revieverNick+" :"+msg+"\r\n")
#define NOTICE_RPLY(senderNick, senderUsername, clientIp, revieverNick, msg)                 std::string(userHostMask(senderNick, senderUsername, clientIp)+" NOTICE "+revieverNick+" :"+msg+"\r\n")
#define RPL_JOIN(senderNick, senderUsername, clientIp, chanName)                             std::string(userHostMask(senderNick, senderUsername, clientIp)+" JOIN "+chanName+"\r\n")
#define RPL_KICK(k_nick, k_username, k_ip, chanName, victim, kickMsg)                        std::string(userHostMask(k_nick, k_username, k_ip)+" KICK "+chanName+" "+victim+" :"+kickMsg+"\r\n")
#define RPL_QUIT(q_nick, q_username, q_ip, quitMsg)                                          std::string(userHostMask(q_nick, q_username, q_ip)+" QUIT :"+quitMsg+"\r\n")
#define RPL_PART(q_nick, q_username, q_ip, chanName, partMsg)                                std::string(userHostMask(q_nick, q_username, q_ip)+" PART "+chanName+" :"+partMsg+"\r\n")
#define RPL_NAMES(hostname, nick, chanName)                                                  std::string(":" + hostname + " 353 " + nick + " = " + chanName + " :")
#define RPL_ENDOFNAMES(hostname, nickname, chanName)                                         std::string(":"+hostname+" 366 "+nickname+" "+chanName+" :End of /NAMES list.\r\n")
#define RPL_MODES(nick, username, clientIp, chanName, msg)                                   std::string(userHostMask(nick, username, clientIp)+" MODE "+chanName+" "+msg+"\r\n")
#define RPL_CHANNELMODEIS(hostname, nick, chanName, msg)                                     std::string(":"+hostname+" 324 "+nick+" "+chanName+" "+msg+"\r\n")
/* >> :gaim!~dd@5.195.225.158 MODE #habexirc -o tesfa */
#define RPL_OPERATORGIVEREVOKE(op_nick, op_username, op_ip, chanName, optype, newopnick)    std::string(userHostMask(op_nick, op_username, op_ip)+" MODE "+chanName+" "+optype+" "+newopnick+"\r\n")
#define RPL_YouIsInvited(op_nick, op_username, op_ip, chanName, inviteeNick)                std::string(userHostMask(op_nick, op_username, op_ip)+" INVITE "+" "+inviteeNick+" "+chanName+" "+"\r\n")
#define RPL_TOPIC(hostname, nick, chanName, msg)                                            std::string(":"+hostname+" 332 "+nick+" "+chanName+" :"+msg+"\r\n")
#define RPL_TOPICNOTSET(hostname, nick, chanName)                                           std::string(":"+hostname+" 331 "+nick+" "+chanName+" :topic is not set\r\n")
#define TOPIC_CHANGE(nick, username, ip, chanName, topicMsg)                                std::string(userHostMask(nick, username, ip)+" TOPIC "+chanName+" :"+topicMsg+"\r\n")

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
std::string                 humanReadableDateTime(void);
std::string                 trim(std::string& str);
#endif // !IRC_HPP