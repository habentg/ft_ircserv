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
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98
RM 			= rm -rf

# Source files:
SRC = source/server.cpp source/utils.cpp source/channel.cpp source/client.cpp source/main.cpp source/command.cpp

# Object files directory:
OBJ = obj

# Object files:
OBJ_FILES = $(addprefix $(OBJ)/, $(SRC:.cpp=.o))

# Rule for generating object files:
$(OBJ)/%.o: %.cpp
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
clean: $(OBJ)
	@echo "Removing Objects"
	$(RM) $(OBJ)

# Rule for cleaning object files, archives and executable:
fclean: clean
	@echo "Removing Executable"
	$(RM) $(NAME)

# Rule for re-making the executable:
re: fclean all
	@echo "Rebuilding Executable"

# Phony targets:
.PHONY: re fclean all clean