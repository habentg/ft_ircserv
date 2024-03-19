/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:44:30 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/22 13:03:00 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

bool    serverRunning = true;

void    signalHandler(int sig) {
    serverRunning = false;
    if (sig == SIGQUIT)
        std::cout << "QUIT\n";
    else if (sig == SIGINT)
        std::cout << "TRL-C\n";
}

/* Program Entry: */
/* 
    -> input validation
    -> Create a single instance of the server
    -> Setting up the server to listen on the given port (service)
    -> Setting up the signal handler for CTRL-QUIT and  CTRL-C signals
    -> Poll system:
        * monitoring our fds for any event
        * waits indefinitely for an event to occur (-1 timeout, the third arg)
    -> we check at which fd does an event occur:
        * if its at the listner socket, it means we have a new connection
        * if its on the other existing sockets, it means we have a message incomming on that respective fd.

*/
int main(int ac, char **av) {
    unsigned short int portNumber = validate_input(ac, av);
    if (portNumber == 0)
        return 1;
    Server *serverObj = Server::createServerInstance(portNumber, std::string(av[2]));
    if (serverObj == NULL)
        return 1;
    try {
        serverObj->server_listen_setup(av[1]);
        signal(SIGINT, signalHandler);
        signal(SIGQUIT, signalHandler);
        while (serverRunning) { 
            if (poll(&(serverObj->getFdArray()[0]), serverObj->getFdArray().size(), -1) == -1 && serverRunning)
                throw std::runtime_error("Error: polling problem");
            if (serverObj->getFdArray()[0].revents == POLLIN) { // we accept the new connection and we add the newfd to our array ....
                serverObj->acceptNewConnection();
                continue ;
            }
            for (size_t i = 1; i < serverObj->getFdArray().size(); i++)
            {
                if (serverObj->getFdArray()[i].revents == POLLIN)
                    serverObj->recieveMsg(serverObj->getFdArray()[i].fd);
                else if (serverObj->getFdArray()[i].revents == 17 || serverObj->getFdArray()[i].revents == 25) {
                    Client *client = serverObj->getClient(serverObj->getFdArray()[i].fd);
                    serverObj->removeClient(client, RPL_QUIT(client->getNickName(), client->getUserName(), client->getIpAddr(), "17/25"));
                }
            }
        }
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    delete serverObj;
    return 0;
}
