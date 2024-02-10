/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 08:44:30 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/11 01:36:41 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

int main(int ac, char **av) {
    // check number of arguments, and validate port number
    double portNumberDouble = validate_input(ac, av);
    if (portNumberDouble == -1)
        return 1;
    // instantiating a single server object and assigning port-number & password 
    Server& serverObj = Server::createServerInstance(portNumberDouble, std::string(av[1]));
    try {
        int sockfd = serverObj.server_listen_setup(av[1]);
        serverObj.addToFdArray(sockfd); // add our listner socket fd to the array so we can watch for new connection request on it.
        
        std::cout << "-------- LISTENING FOR CONNECTIONS .... ---------\n";

        while (1) { // our infinite loop (server main loop)
            // monitoring our fds for any event.
            // this will basiclly block untill an "event" happens
            // it waits indefinitely for an event to occur (-1 timeout, the third arg)
                //-> for now let it run for indefinitely but later we will add "pinging" feature i.e we will check on our clients after certain ammount of time, and we will desconnect them if they are not active or something.
            if (poll(&(serverObj.getFdArray()[0]), serverObj.getFdArray().size(), -1) == -1)
                throw std::runtime_error("Error polling problem");
            // then we check at which fd does the event occur i.e the POLLIN event
                // -> if its at the listner fd, it means we have a new connection
                // -> if its on the other existing fds, it means we have a message incomming on that respective fd.
            if (serverObj.getFdArray()[0].revents == POLLIN) {
                // we accept the new connection and we add the newfd to our array ....
                serverObj.acceptNewConnection();
            } else {
                // we have to indetify the fd the event occured so we can "recv".
                std::vector<pollfd>::iterator it = serverObj.getFdArray().begin();
                for (; it != serverObj.getFdArray().end(); ++it) {
                    if ((*it).revents == POLLIN) {
                        std::cout << "this is from an existing client\n";
                        serverObj.recieveMsg((*it).fd);
                        break ;
                    }
                }
            }
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    } catch (...) {
        std::cerr << "ERROR!!" << std::endl;
    }
    delete &serverObj;
    return 0;
}
