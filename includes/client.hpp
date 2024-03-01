/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/10 23:58:54 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/22 13:00:37 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "irc.hpp"

class Server;
class Channel;

class Client {
    private:
        
        int         _clientFd;
        bool        _isConnected;
        bool        _isAuthenticated;
        std::string _NICK;
        std::string _USERNAME;
        std::string _REALNAME;
        char*       _ipaddr;
        uint32_t    _port;
        unsigned int _wrongPassCount;
        std::set<std::string> _channelsJoined;
        struct sockaddr* _clientInfo; // incase I need more info about the client
        Client(void);
    public:
        Client(int clientFd, struct sockaddr *clientInfo);
        ~Client(void);
        // getters and setters
        int             getFd(void) const;
        
        bool            IsClientConnected(void) const;
        void            setIsregistered(bool value);
        
        bool            getIsAuthenticated(void) const;
        void            setIsAuthenticated(bool value);
        
        std::string     getNickName(void) const;
        void            setNICK(std::string value);
        
        std::string     getUserName(void) const;
        void            setUserName(std::string value);
        
        std::string     getRealName(void) const;
        void            setRealName(std::string value);
        
        std::string     getIpAddr(void) const;
        
        std::set<std::string>& getChannelsJoined(void);

        unsigned int    getWrongPassCount(void) const;
        void            setWrongPassCount(unsigned int value);
        void            addChannelNameToCollection(std::string chanName);
};

#endif // !CLIENT_HPP