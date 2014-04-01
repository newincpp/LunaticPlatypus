SRCDIR	=exemple
SRC	=$(SRCDIR)/main.cpp
EXT     =cpp
NAME	=gl
CXXFLAGS= -Wall -Wextra -W -std=c++0x -I./GLnewin
LDFLAGS	= GLnewin.a -lGL -lX11
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o $(NAME)

rl: $(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o $(NAME)

glnewin:
	make -C GLnewin

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re

