/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:33:52 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/18 15:23:27 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "irc.hpp"

class Client;

class Server {
    private:
        int                         _sockfd;
        const unsigned short int    _port_number;
        std::string                 _passwd;
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
        unsigned short int getServerPortNumber(void) const;
        std::vector<struct pollfd>& getFdArray(void);
        void                        addToFdArray(int newfd);
        void                        acceptNewConnection(void);
        void                        recieveMsg(int clientFd);
        void                        server_listen_setup(char *portNumber);
        void                        saveClientINfo(int clientFd, struct sockaddr *clientInfo);
        void                        sendMsgToClient(int clientFd, std::string msg);
        void                        sendMsgToAllClients(std::string msg);
        void                        registerClient(int clientFd, std::string msg);
        Client*                     getClient(int clientFd);
        void                        removeClient(int clientFd);
        std::string                 getPassword(void);
        void                        CAPauth(Client* cl, std::string cap);
        void                        doStuff(int clientFd, std::string msg);
        class exc : public std::exception {
            public:
                const char* what() const throw();
        };
};

double validate_input(int ac, char **av);

#endif //!SERVER_HPP