NAME = webserv
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRCS =	main.cpp \
		server/server.cpp \
		event/eventManager.cpp \
		console/console.cpp \
		config/location.cpp \
		config/ConfigParser.cpp \
		config/WebservConfig.cpp \
		config/file_reader.cpp\
		messages/HttpMessage.cpp \
		messages/HttpHeaders.cpp \
		messages/HttpResponse.cpp \
		messages/HttpRequest.cpp \
		parsing/Normalization.cpp

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
