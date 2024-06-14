NAME		:= webserv

SRCS 		:= $(addprefix src/, \
	main.cpp \
	ConfigParser.cpp \
	CgiHandler.cpp \
	CgiProcess.cpp \
	FastCgiHandler.cpp \
	Location.cpp \
	Peer.cpp \
	Request.cpp \
	Response.cpp \
	Server.cpp \
	ServerConfig.cpp \
	WebServ.cpp \
)
OBJS		:= $(SRCS:.cpp=.o)

PID_FILE = var/php-fpm.d/tmp/php-fpm.pid

CC 			:= c++
CFLAGS		:= -Wall -Wextra -Werror -g -std=c++98 -Iinc/ -O3

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

php-fpm:
	bash var/fastCgiInit.sh

php-fpm-clean:
	@if [ -f "$(PID_FILE)" ]; then \
		PID=$$(cat "$(PID_FILE)"); \
		if [ "$$PID" -eq "$$PID" ] 2>/dev/null; then \
			echo "Killing process with PID: $$PID"; \
			kill "$$PID"; \
			if [ $$? -eq 0 ]; then \
				echo "Process $$PID has been terminated."; \
			else \
				echo "Failed to terminate process $$PID."; \
			fi \
		else \
			echo "Invalid PID: $$PID"; \
		fi \
	else \
		echo "PID file not found: $(PID_FILE)"; \
	fi
	rm -rf var/php-fpm.d/

.PHONY : clean fclean re info- php-fpm
.SILENT :
