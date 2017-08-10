LUNATICPLATYPUSDIR=../LunaticPlatypus
include  $(LUNATICPLATYPUSDIR)/project.mk
SRC	+= main.cpp \
	   StaticClassTest.cpp

NAME	=demo

OBJS	= $(SRC:.$(EXT)=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER)  $(OBJS) -o $(NAME) $(LDFLAGS)

relink: $(OBJS)
	$(LINKER)  $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
