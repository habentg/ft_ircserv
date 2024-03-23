/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   bot.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkiflema <mkiflema@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 13:49:54 by tpetros           #+#    #+#             */
/*   Updated: 2024/03/23 10:57:35 by mkiflema         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

# include <iostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <cstring>
# include <vector>
# include <algorithm>
# include <sstream>
# include <exception>
#include <cstring>
#include <stdexcept>
#include <poll.h>
#include <csignal>

class Bot 
{
	private:
		int 								sockfd;
		struct sockaddr_in 					serverAddr;
		std::vector<std::string> 			dictionary;
		const static std::string			nickName;
		const static std::string			realName;
		const static std::string			userName;

	public:
		Bot(const std::string& serverIP, int port, const std::string& password);
		~Bot( void );

		void 			setDictionary( void );
		void 			establishConnection( void );
		void 			joinChannel( const std::string& channel );
		bool 			isOffensiveWord( const std::string& word );
		bool 		checkOffensiveWords( const std::string& message );
		void 			monitor( std::string channel );
		void 			kickUser( const std::string& user, std::string channel );
		void 			sendMsg( const std::string& message );
};

std::string trim(const std::string &s);
std::vector<std::string> split(const std::string& str, char delimiter);
std::string trimm(std::string& str);
