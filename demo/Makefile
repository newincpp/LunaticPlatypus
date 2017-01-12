SRC	= main.cpp

EXT=cpp
NAME	=demo
LUNATICPLATIPUSDIR=../GLnewin
OPTIM= -march=native -O2 -fno-rtti #-fno-exceptions
INCLUDE=-I$(LUNATICPLATIPUSDIR)/ -I$(LUNATICPLATIPUSDIR)/imgui/ -I$(LUNATICPLATIPUSDIR)/glm
CXXFLAGS= -Wall -Wextra -W -std=c++14 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
LIBS= -lGL -lglfw -ldl -pthread \
      libLunaticPlatipus.a \
      $(LUNATICPLATIPUSDIR)/libs/libHalf.a \
      $(LUNATICPLATIPUSDIR)/libs/libhdf5.a \
      /usr/lib/libIex.a  \
      $(LUNATICPLATIPUSDIR)/libs/libAlembic.a
LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS)
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)
INSTALL = ../demo

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER)  $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
