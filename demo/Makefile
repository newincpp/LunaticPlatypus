SRC	= main.cpp

OS	:= $(shell uname -s)
EXT=cpp
NAME	=demo
LUNATICPLATYPUSDIR=../GLnewin
OPTIM= -march=native -O2 -fno-rtti #-fno-exceptions
INCLUDE=-I$(LUNATICPLATYPUSDIR)/ -I$(LUNATICPLATYPUSDIR)/imgui/ -I$(LUNATICPLATYPUSDIR)/glm
CXXFLAGS= -Wconversion -Wall -Wextra -W -std=c++14 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)

LIBS= -ldl -pthread libLunaticPlatypus.a
ifeq ($(OS),Linux)
    LIBS+=-lGL -lrt -lm -lXrandr -lXinerama -lXi -lXcursor -lXrender -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lX11 -lXxf86vm
endif
ifeq ($(OS),Darwin)
    LIBS+= -framework openGL 
endif
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
