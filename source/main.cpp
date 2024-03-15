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

bool    stopServer = false; // global variable to stop the server when CTRL-C is pressed

void    signalHandler(int sig) {
    stopServer = true;
    if (sig == SIGQUIT)
        std::cout << "QUIT\n";
    else if (sig == SIGINT)
        std::cout << "TRL-C\n";
}

/* Program Entry: */
int main(int ac, char **av) {
    // check number of arguments, and validate port number
    unsigned short int portNumber = validate_input(ac, av);
    if (portNumber == 0)
        return 1;
    // instantiating a single server object and assigning port-number & password 
    Server *serverObj = Server::createServerInstance(portNumber, std::string(av[2]));
    if (serverObj == NULL)
        return 1;
    try {
        serverObj->server_listen_setup(av[1]); // setting up the server to listen on the given port (service)
        while (1) { 
            signal(SIGINT, signalHandler); // setting up the signal handler for CTRL-C
            signal(SIGQUIT, signalHandler); // setting up the signal handler for CTRL-QUIT
            // monitoring our fds for any event.
            // this will basiclly block untill an "event" happens
            // it waits indefinitely for an event to occur (-1 timeout, the third arg)
                //-> for now let it run for indefinitely but later we will add "pinging" feature i.e we will check on our clients after certain ammount of time, and we will desconnect them if they are not active or something.
            if (poll(&(serverObj->getFdArray()[0]), serverObj->getFdArray().size(), -1) == -1 && !stopServer)
                throw std::runtime_error("Error: polling problem");
            if (stopServer)
                break ;
            // then we check at which fd does the event occur i.e the POLLIN event
                // -> if its at the listner fd, it means we have a new connection
                // -> if its on the other existing fds, it means we have a message incomming on that respective fd.
            if (serverObj->getFdArray()[0].revents == POLLIN) { // we accept the new connection and we add the newfd to our array ....
                serverObj->acceptNewConnection();
            } else {
                size_t i = 0;
                while (++i < serverObj->getFdArray().size() && !stopServer){
                    if (serverObj->getFdArray()[i].revents == POLLIN)
                        serverObj->executeMsg(serverObj->getFdArray()[i].fd); // we will "read" from that fd
                    else if (serverObj->getFdArray()[i].revents == 17) { // for ctrl-c int the cliebt
                        Client *client = serverObj->getClient(serverObj->getFdArray()[i].fd);
                        std::string quitMsg = RPL_QUIT(client->getNickName(), client->getUserName(), client->getIpAddr(), "QUIT");
                        serverObj->removeClient(client, quitMsg);
                    }
                }
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    serverObj->sendMsgToAllClients(Conn_closed(serverObj->getHostname()));
    delete serverObj;
    return 0;
}
