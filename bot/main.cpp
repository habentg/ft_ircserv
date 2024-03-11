/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpetros <tpetros@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/07 14:53:37 by tpetros           #+#    #+#             */
/*   Updated: 2024/03/07 14:53:40 by tpetros          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "bot.hpp"

void parse_args( int ac, char **av, int &port, std::string& serverIp, std::string &password, std::string &channel )
{
	if (ac != 5)
	{
		std::cout << "Invalid parameter for launching bot\n";
		std::exit(EXIT_FAILURE);
	}
    serverIp = std::string(av[1]);
	char *endPtr;
    double pNumberDouble = std::strtod(av[2], &endPtr);
    if (*endPtr != '\0' || pNumberDouble < 1024 || pNumberDouble > 65535)
	{
        std::cerr << "Invalid port number of launching bot\n" << std::endl;
        std::exit(EXIT_FAILURE);
    }
	port = static_cast<int>(pNumberDouble);
	password = std::string(av[3]);
    channel = av[4];
}

int main(int ac, char **av) 
{
    std::string serverIp;
    int port; 
	std::string password;
    std::string channel;

	parse_args(ac, av, port, serverIp, password, channel);
	if (password.empty())
	{
		std::cerr << "Password cannot be empty!\n";
		std::exit(EXIT_FAILURE);
	}
	try
	{
		Bot bot(serverIp, port, password);
		bot.joinChannel(channel);
    	bot.monitor(channel);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

    return 0;
}
