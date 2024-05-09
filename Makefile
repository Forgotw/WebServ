NAME		:= webserv

SRCS 		:= $(addprefix src/, \
	main.cpp \
	ConfigParser.cpp \
	handleCGI.cpp \
	Location.cpp \
	Peer.cpp \
	Request.cpp \
	Response.cpp \
	Server.cpp \
	ServerConfig.cpp \
	WebServ.cpp \
)
OBJS		:= $(SRCS:.cpp=.o)

CC 			:= c++
CFLAGS		:= -Wall -Wextra -Werror -std=c++98 -Iinc/ -O3

RM			:= rm -f
MAKEFLAGS   += --no-print-directory

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(OBJS) $(CFLAGS) -o $(NAME)
	$(info CREATED $(NAME))

%.o : %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	$(info CREATED $@)

clean :
	$(RM) $(OBJS)
	$(info DELETED objects files)

fclean : clean
	$(RM) $(NAME)
	$(info DELETED $(NAME))

re :
	$(MAKE) fclean
	$(MAKE) all

info-%:
	$(MAKE) --dry-run --always-make $* | grep -v "info"

.PHONY : clean fclean re info-
.SILENT :
