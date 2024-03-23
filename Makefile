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
BOT         = bigbrother

pwd = $(shell pwd):/home/vscode/src

# Compiler, flags and RM command:
CXX 		= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++98
RM 			= rm -rf


# Source files:
SRC = $(addprefix source/, server.cpp utils.cpp channel.cpp client.cpp main.cpp command.cpp server_connnection_registration.cpp channel_modes.cpp)
BONUS_SRC = bot/bot.cpp bot/main.cpp bot/utils.cpp

# Object files directory:
OBJ = obj

# Object files:
OBJ_FILES = $(patsubst %.cpp, $(OBJ)/%.o, $(notdir $(SRC)))
BONUS_OBJS = ${BONUS_SRC:.cpp=.o}

# Rule for generating object files:
$(OBJ)/%.o: source/%.cpp
	@mkdir -p $(dir $@)
	@echo "	~ Making object file [$(notdir $@)] from source file {$(notdir $<)} ..."
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule for generating the executable:
all: $(NAME)

# Rule for generating the executable:
$(NAME): $(OBJ_FILES)
	@echo "Creating ircserv...."
	@$(CXX) $(CXXFLAGS) $(OBJ_FILES) -o $@
	@echo "successfully created! Enjoy my friend"

# Rule for generating the bonus executable:
$(BOT) : $(BONUS_OBJS)
	$(CXX) $(CXXFLAGS) $(BONUS_OBJS) -o $(BOT)

# Rule for cleaning object files:
clean:
	$(RM) $(OBJ)
	$(RM) $(BONUS_OBJS)

# Rule for cleaning object files, archives and executable:
fclean: clean
	$(RM) $(NAME) $(BOT)

# Rule for re-making the executable:
re: fclean all

bonus: $(BOT)

# Phony NAMEs:
.PHONY: re fclean all clean bonus