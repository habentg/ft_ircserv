/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:33:52 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/05 15:04:18 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

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

class Server {
    private:
        const unsigned short int  _port_number;
        std::string         _passwd;
        static  Server*     serverInstance;
        Server(void);
        Server(unsigned short int portNumberDouble, std::string password);
        Server(const Server& cpy);
        Server& operator=(const Server& cpy);
    public:
        ~Server(void);
        static Server& createServerInstance(double portNumberDouble, std::string password);
        unsigned short int getServerPortNumber(void) const;
        int server_listen_setup(char *portNumber);
        class exc : public std::exception {
            public:
                const char* what() const throw();
        };
};

double validate_input(int ac, char **av);

#endif //!SERVER_HPP