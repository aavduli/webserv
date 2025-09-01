# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jim <jim@student.42.fr>                    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/09/01 11:10:05 by jim               #+#    #+#              #
#    Updated: 2025/09/01 12:53:19 by jim              ###   ########.fr        #
#                                                                              #
# **************************************************************************** #


NAME = program
CXX = c++
CXXFLAGS = -Wall -Werror -Wextra -std=c++98
SRCS =	config.cpp\
		mainconf.cpp\
		parser_config.cpp\
		location.cpp

OBJS = $(SRCS:.cpp=.o)

all:$(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re
