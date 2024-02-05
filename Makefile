# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/04 08:47:28 by hatesfam          #+#    #+#              #
#    Updated: 2024/02/05 16:29:10 by hatesfam         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= ft_ircserv

CXX 		= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -MMD -MP
RM 			= rm -rf

SRC_PATH	= source
OBJ_PATH	= objects
SRC_SERVER	= source/server/server.cpp source/server/utils.cpp
SRC_CHANNEL	= source/channel/channel.cpp
SRC_MAIN	= source/main.cpp

SRC = $(SRC_SERVER) \
       $(SRC_CHANNEL) \
       $(SRC_MAIN)

OBJS = $(SRC:source/%.c=$(OBJ_PATH)/%.o)

all: $(OBJ_PATH) $(NAME)

$(NAME): $(OBJS)
	@echo "Compiling and Linking Ft_IRCserv"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)
	@echo "Compilation successfull. Enjoy"

$(OBJ_PATH):
	@mkdir -p $(OBJ_PATH)/server
	@mkdir -p $(OBJ_PATH)/channel

$(OBJ_PATH)/%.o: $(SRC_PATH)%.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

	
clean:
	@$(RM) $(OBJ_PATH)

fclean: clean
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
