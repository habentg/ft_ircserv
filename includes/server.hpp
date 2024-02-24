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
class Command;

class Server {
    private:
        int                         _serverSocketFd;
        const unsigned short int    _serverPort;
        std::string                 _serverPassword;
        std::string                 _serverHostName;
        std::vector<struct pollfd>  _fdsArray; // fds of all connected clients including the listening socket fd.
        std::map<int, Client *>     _clients; // our users
        static  Server*             serverInstance; // pointer to our server instance

        // constructors
        Server(void);
        Server(unsigned short int portNumberDouble, std::string password);
        Server(const Server& cpy);
        Server& operator=(const Server& cpy);
    public:
        ~Server(void);
        static Server& createServerInstance(double portNumberDouble, std::string password);
        /* getters */
        unsigned short int getServerPortNumber(void) const;
        std::vector<struct pollfd>& getFdArray(void);
        std::string                 getPassword(void) const;
        std::string                 getServerHostName(void) const;
        Client*                     getClient(int clientFd);
        size_t                      getNumberOfClients(void) const;
        /* methods */
        void                        addToFdArray(int newfd);
        void                        acceptNewConnection(void);
        void                        recieveMsg(int clientFd);
        void                        server_listen_setup(char *portNumber);
        void                        saveClientINfo(int clientFd, struct sockaddr *clientInfo);
        void                        sendMsgToClient(int clientFd, std::string msg);
        void                        sendMsgToAllClients(std::string msg);
        void                        registerClient(Client *client, Command *command);
        void                        removeClient(int clientFd);
        int                        isClientAvailable(int clientFd, std::string nick) const;
        void                        userAuthenticationAndWelcome(Client* cl, Command *command);
        bool                        authenticateClient(Client *cl, Command *command);
        void                        doStuff(Client* client, Command *command);
        class exc : public std::exception {
            public:
                const char* what() const throw();
        };
};

#endif //!SERVER_HPP