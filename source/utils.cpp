/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 11:30:21 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/21 18:27:09 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/irc.hpp"

unsigned short int validate_input(int ac, char **av) {
    if (ac != 3) {
        std::cerr << INCORRECT_ARGS << std::endl;
        return 0;
    }
    char *endPtr;
    double pNumberDouble = std::strtod(av[1], &endPtr);
    if (*endPtr != '\0' || pNumberDouble < 1024 || pNumberDouble > 65535) {
        std::cerr << INVALID_serverPort << std::endl;
        return 0;
    }
    if (std::string(av[2]).length() < 4 || std::string(av[2]).length() > 15) {
        std::cerr << PASSWORD_TOO_SHORT << std::endl;
        return 0;
    }
    return pNumberDouble;
}

std::string trim(std::string& str) {
    int len = str.length();
    int i = 0;
    for (; i < len; i++)
    {
        if (str[i] != '\n' && str[len - 1] != '\r' && str[len - 1] != 10 && str[i] != ' ')
            break;
    }
    for (; len >= 0; len--)
    {
        if (str[len - 1] != '\n' && str[len - 1] != '\r' && str[len - 1] != 10 && str[len - 1] != ' ')
            break;
    }
    if (i >= len)
        return "";
    return  str.substr(i, len - i);
}

std::vector<std::string> split(std::string& str, char delimiter) {
    std::vector<std::string> arr;
    std::string elem;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == delimiter || str[i] == 10 || str[i] == 13) {
            if (elem.empty())
                continue ;
            arr.push_back(trim(elem));
            elem = "";
        } else {
            elem += str[i];
        }
    }
    if (!elem.empty())
        arr.push_back(trim(elem)); // Add the last element
    return arr;
}

void printVector(std::vector<std::string> vec) {
    std::cout << "[";
    std::vector<std::string>::iterator it = vec.begin();
    for (; it != vec.end(); ++it) {
        if (it != vec.begin())
            std::cout << ", ";
        std::cout << *it;
    }
    std::cout << "]\n";
}

std::string humanReadableDateTime(void) {
     // Get current time
    std::time_t currentTime = std::time(NULL);
    
    // Convert current time to std::tm structure
    std::tm* timeInfo = std::localtime(&currentTime);
    
    // Format timeInfo into a string
    char buffer[80];
    std::strftime(buffer, 80, "%c", timeInfo);    
    return std::string(buffer);
}