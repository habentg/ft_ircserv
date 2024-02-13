/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 23:58:54 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/13 07:12:51 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Client {
    private:
        
        int         _clientFd;
        bool        _allInfoRecieved;
        bool        _isAuthenticated;
        std::string _NICK;
        std::string _USERNAME;
        char*       _ipaddr;
        uint32_t    _port;
        struct sockaddr* _clientInfo; // incase I need more info about the client
        Client(void);
    public:
        Client(int clientFd, struct sockaddr *clientInfo);
        ~Client(void);
        // getters and setters
        bool getAllInfoRecieved(void) const;
        void setAllInfoRecieved(bool value);
        bool getIsAuthenticated(void) const;
        void setIsAuthenticated(bool value);
        std::string getNICK(void) const;
        void setNICK(std::string value);
        std::string getUserName(void) const;
        void setUserName(std::string value);
};

#endif // !CLIENT_HPP