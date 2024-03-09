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
        std::vector<struct pollfd>          _fdsArray; // fds of all connected clients including the listening socket fd.
        std::map<int, Client *>             _clients; // our users
        std::map<std::string, int>          _nick_fd_map; // nickname-fd map, to optimize client look up for sending
        std::map<std::string, Channel *>    _channels; // all the channels in this server (name-channel format)
        std::set<std::string>               _serverOperators;
        static  Server*                     serverInstance; // pointer to our server instance

        // constructors
        // Server(void);
        Server(unsigned short int portNumber, std::string password);
        // Server(const Server& cpy);
        // Server& operator=(const Server& cpy);
    public:
        ~Server(void);
        static Server *createServerInstance(unsigned short int portNumber, std::string password);
        /* getters */
        unsigned short int          getServerPortNumber(void) const;
        std::vector<struct pollfd>& getFdArray(void);
        std::string                 getPassword(void) const;
        std::string                 getServerHostName(void) const;
        Client*                     getClient(int clientFd);
        Client*                     getClientByNick(std::string nick);
        size_t                      getNumberOfClients(void) const;
        /* server op methods */
        void                        server_listen_setup(char *portNumber);
        void                        addToFdArray(int newfd);
        void                        acceptNewConnection(void);
        void                        registerClient(Client *client, Command *command);
        bool                        authenticateClient(Client *cl, Command *command);
        void                        userAuthenticationAndWelcome(Client* cl, Command *command);
        void                        recieveMsg(int clientFd);
        int                         isClientAvailable(std::string nick) const;
        void                        sendMsgToClient(int clientFd, std::string msg);
        void                        sendMsgToAllClients(std::string msg);
        void                        doStuff(Client* client, Command *command);
        std::string                 constructReplayMsg(std::string senderNick, Client *senderClient, std::string recieverNick, std::string msgToSend);
        void                        removeClient(Client *senderClient, std::string quitMsg);
        std::string                 isClientServerOp(std::string nick);

        /* channel related methods */ // because its the duity of the server to do operation on the channel when a user requests with the key(chanName)
        void    channelRelatedOperations(Client* client, Command *command);
        
        void      createChannel(std::string chanName, Client *creator, Command *command); // gonna call the constructor of the channel class
        Channel   *getChanByName(std::string chanName);
        void      deleteAChannel(Channel *chan);
        void      removeClientFromChan(std::string victimNick, Channel *chan);
        void       sendMessageToChan(Channel *chan, std::string sender, std::string msg, bool chanNotice);
        
};

#endif //!SERVER_HPP