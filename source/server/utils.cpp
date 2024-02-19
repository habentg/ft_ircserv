/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 11:30:21 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/18 11:43:36 by hatesfam         ###   ########.fr       */
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

std::vector<std::string> newline_split(std::string& str) {
    std::vector<std::string> arr;
    std::string::size_type start = 0;
    std::string::size_type end = 0;
    
    while ((end = str.find(13, start)) != std::string::npos) {
        arr.push_back(str.substr(start, end - start));
        start = end + 2; // Skip the newline character
    }
    if (start < str.length()) {
        arr.push_back(str.substr(start));
    }
    
    return arr;
}

std::vector<std::string> split(std::string& str, char delimiter) {
    std::vector<std::string> arr;
    std::string elem;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == delimiter) {
            arr.push_back(elem);
            elem = "";
        } else {
            elem += str[i];
        }
    }
    arr.push_back(elem); // Add the last element
    return arr;
}