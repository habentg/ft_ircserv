/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/04 09:38:46 by hatesfam          #+#    #+#             */
/*   Updated: 2024/02/04 12:44:43 by hatesfam         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERROR_HPP
# define ERROR_HPP

# define INCORRECT_ARGS "Error: Incorrect Numner of arguments!!\n    Usage: ./ft_ircserv <port number <password>"
# define INVALID_PORT_NUMBER "\nError: Invalid Port Number Recieved!!\n\n    Should be Numberical value & between 0 - 65535\n"
# define PASSWORD_TOO_SHORT "\nError: Password to short!!\n\n    Password should be at least 4 characters\n"

#endif // !ERROR_HPP