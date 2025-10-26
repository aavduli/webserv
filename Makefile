NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS =	main.cpp \
		server/server.cpp \
		server/onConnection.cpp \
		server/eventManager.cpp \
		server/NetworkHandler.cpp \
		server/ConnectionManager.cpp \
		server/EventProcessor.cpp \
		console/console.cpp \
		status/status.cpp \
		config/ConfigParser.cpp \
		config/ConfigValidator.cpp \
		config/FileReader.cpp \
		config/WebservConfig.cpp \
		config/ParsingUtils.cpp \
		messages/data/HttpMessage.cpp \
		messages/data/HttpResponse.cpp \
		messages/data/HttpRequest.cpp \
		messages/data/RequestUri.cpp \
		messages/parsing/RequestParser.cpp \
		messages/handling/MessageHandler.cpp \
		messages/handling/RequestValidator.cpp \
		messages/handling/RequestProcessor.cpp \
		messages/handling/ResponseGenerator.cpp \
		parsing/Normalization.cpp \
		parsing/Conversion.cpp \
		parsing/Validation.cpp \
		parsing/Paths.cpp \
		cgi/CgiExec.cpp

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
	rm -rf ./logs/*
	rm -rf ./www/file_manager/files

re: fclean all

.PHONY: all clean fclean re
