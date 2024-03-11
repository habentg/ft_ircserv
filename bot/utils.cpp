/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tpetros <tpetros@student.42abudhabi.ae>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/08 19:16:29 by tpetros           #+#    #+#             */
/*   Updated: 2024/03/08 19:16:30 by tpetros          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "bot.hpp"

std::string trim(const std::string &s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && (isspace(*it) || *it == '\t' || *it == '\r' || *it == '\n'))
        it++;

    std::string::const_reverse_iterator rit = s.rbegin();
    while (rit.base() != it && (isspace(*rit) || *rit == '\t' || *it == '\r' || *it == '\n'))
        rit++;

    return std::string(it, rit.base());
}


std::vector<std::string> split(const std::string& str, char delimiter) 
{
    std::vector<std::string> arr;
    std::string elem;
    for (size_t i = 0; i < str.length(); ++i) 
	{
        if (str[i] == delimiter || str[i] == 10 || str[i] == 13) 
		{
            if (elem.empty())
                continue ;
            arr.push_back(trim(elem));
            elem = "";
        } else
            elem += str[i];
    }
    if (!elem.empty())
        arr.push_back(trim(elem));
    return arr;
}

