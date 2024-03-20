/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:33:52 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/22 14:36:33 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "irc.hpp"

class Client;
class Channel;
class Command;

class Server {
    private:
        int                                 _serverSocketFd;
        const unsigned short int            _serverPort;
        std::string                         _serverPassword;
        std::string                         _serverHostName;
        std::vector<struct pollfd>          _fdsArray;
        std::map<int, Client *>             _clients;
        std::map<std::string, int>          _nick_fd_map;
        std::map<std::string, Channel *>    _channels;
        static  Server*                     serverInstance; 
        Server(unsigned short int portNumber, std::string password);
    public:
        ~Server(void);
        static Server               *createServerInstance(unsigned short int portNumber, std::string password);
        unsigned short int          getServerPortNumber(void) const;
        std::vector<struct pollfd>& getFdArray(void);
        std::string                 getPassword(void) const;
        std::string                 getHostname(void) const;
        Client*                     getClient(int clientFd);
        Client*                     getClientByNick(std::string nick);
        size_t                      getNumberOfClients(void) const;
        void                        server_listen_setup(char *portNumber);
        void                        addToFdArray(int newfd);
        void                        acceptNewConnection(void);
        void                        registerClient(Client *client, Command *command);
        bool                        authenticateClient(Client *cl, Command *command);
        void                        userAuthenticationAndWelcome(Client* cl, Command *command);
        void                        recieveMsg(int clientFd);
        void                        executeMsg(Client *client);
        int                         getClientFd(std::string nick) const;
        void                        sendMsgToClient(int clientFd, std::string msg);
        void                        doStuff(Client* client, Command *command);
        void                        removeClient(Client *senderClient, std::string quitMsg);
        void                        createChannel(std::string chanName, Client *creator, Command *command);
        Channel                     *getChannel(std::string chanName);
        void                        deleteAChannel(Channel *chan);
        void                        removeClientFromChan(Client *victim, Channel *chan);
        void                        forwardMsgToChan(Channel *chan, std::string sender, std::string msg, bool isChanNotice);
        void                        namesCmd(Client *client, std::string chanName);
        const std::map<int, Client*>& getAllClients() const;
        std::map<std::string, Channel *>& getAllChannels( void );
};

#endif //!SERVER_HPP