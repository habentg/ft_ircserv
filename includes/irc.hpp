/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   irc.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:41:20 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/13 04:42:51 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IRC_HPP
# define IRC_HPP

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

# include "client.hpp"
# include "server.hpp"
# include "error.hpp"
# include "channel.hpp"

#endif // !IRC_HPP