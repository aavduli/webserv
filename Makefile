NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS =	main.cpp \
		server/server.cpp \
		server/onConnection.cpp \
		event/eventManager.cpp \
		console/console.cpp \
		config/config.cpp \
		config/location.cpp \
		config/parser_config.cpp \
		config/webserv_config.cpp \
		messages/data/HttpMessage.cpp \
		messages/data/HttpResponse.cpp \
		messages/data/HttpRequest.cpp \
		messages/data/RequestUri.cpp \
		messages/handling/MessageHandler.cpp \
		messages/parsing/MessageParser.cpp \
		messages/parsing/RequestParser.cpp \
		parsing/Normalization.cpp \
		parsing/Conversion.cpp \
		parsing/Validation.cpp
		
OBJDIR = obj

OBJS = $(SRCS:.cpp=.o)
OBJS := $(addprefix $(OBJDIR)/, $(notdir $(OBJS:.o=.o)))

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/%.o:
	$(CXX) $(CXXFLAGS) -c $(filter %/$*.cpp,$(SRCS)) -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
