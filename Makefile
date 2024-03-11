# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: hatesfam <hatesfam@student.42abudhabi.a    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/04 08:47:28 by hatesfam          #+#    #+#              #
#    Updated: 2024/02/21 13:09:10 by hatesfam         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

# Executable name:
NAME		= ircserv

# Compiler, flags and RM command:
CXX 		= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98 -g
RM 			= rm -rf

# Source files:
SRC = $(addprefix source/, server.cpp utils.cpp channel.cpp client.cpp main.cpp command.cpp server_connnection_registration.cpp channel_modes.cpp)

# Object files directory:
OBJ = obj

# Object files:
OBJ_FILES = $(patsubst %.cpp, $(OBJ)/%.o, $(notdir $(SRC)))

# Rule for generating object files:
$(OBJ)/%.o: source/%.cpp
	@mkdir -p $(dir $@)
	@echo "	~ Making object file [$(notdir $@)] from source file {$(notdir $<)} ...\n"
	@$(CXX) $(FLAGS) -c $< -o $@

# Rule for generating the executable:
all: $(NAME)

# Rule for generating the executable:
$(NAME): $(OBJ_FILES)
	@echo "Compiling and Linking Ft_IRCserv"
	@$(CXX) $(FLAGS) $(OBJ_FILES) -o $@
	@echo "Compilation successfull. Enjoy"

# Rule for cleaning object files:
clean:
	@echo "Removing Objects"
	$(RM) $(OBJ)

# Rule for cleaning object files, archives and executable:
fclean: clean
	@echo "Removing Executable"
	$(RM) $(NAME) bigbrother

# Rule for re-making the executable:
re: fclean all
	@echo "Rebuilding Executable"

bot:
	cd bot/ && ${CXX} ${FLAGS} bot.cpp main.cpp utils.cpp -o ../bigbrother && cd - 

# Phony targets:
.PHONY: re fclean all clean bot