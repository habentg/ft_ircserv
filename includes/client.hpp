/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 23:58:54 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/19 13:50:31 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Client {
    private:
        
        int         _clientFd;
        bool        _isRegistered;
        bool        _isAuthenticated;
        std::string _NICK;
        std::string _USERNAME;
        std::string _REALNAME;
        std::string _HOSTNAME;
        char*       _ipaddr;
        uint32_t    _port;
        struct sockaddr* _clientInfo; // incase I need more info about the client
        Client(void);
    public:
        Client(int clientFd, struct sockaddr *clientInfo);
        ~Client(void);
        // getters and setters
        bool getIsregistered(void) const;
        void setIsregistered(bool value);
        bool getIsAuthenticated(void) const;
        int getClientFd(void) const;
        void setIsAuthenticated(bool value);
        std::string getNICK(void) const;
        void setNICK(std::string value);
        std::string getUserName(void) const;
        void setUserName(std::string value);
        std::string getHostName(void) const;
        void setHostName(std::string value);
        std::string getRealName(void) const;
        void setRealName(std::string value);
        std::string getIpAddr(void) const;
};

#endif // !CLIENT_HPP