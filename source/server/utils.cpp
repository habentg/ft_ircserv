/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 11:30:21 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/13 04:48:03 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/irc.hpp"

double validate_input(int ac, char **av) {
    if (ac != 3) {
        std::cerr << INCORRECT_ARGS << std::endl;
        return -1;
    }
    char *endPtr;
    double pNumberDouble = std::strtod(av[1], &endPtr);
    if (*endPtr != '\0' || pNumberDouble < 1024 || pNumberDouble > 65535) {
        std::cerr << INVALID_PORT_NUMBER << std::endl;
        return -1;
    }
    if (std::string(av[2]).length() < 4) {
        std::cerr << PASSWORD_TOO_SHORT << std::endl;
        return -1;
    }
    return pNumberDouble;
}
