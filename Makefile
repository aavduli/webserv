NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS =	main.cpp \
		server/server.cpp \
		server/onConnection.cpp \
		event/eventManager.cpp \
		console/console.cpp \
		config/ConfigLocation.cpp \
		config/ConfigParser.cpp \
		config/ConfigValidator.cpp \
		config/FileReader.cpp \
		config/WebservConfig.cpp \
		messages/data/HttpMessage.cpp \
		messages/data/HttpResponse.cpp \
		messages/data/HttpRequest.cpp \
		messages/data/RequestUri.cpp \
		messages/handling/MessageHandler.cpp \
		messages/parsing/MessageParser.cpp \
		messages/parsing/RequestParser.cpp \
		parsing/Normalization.cpp \
		parsing/Conversion.cpp

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
