SRC	= nelo.cpp

OS	:= $(shell uname -s)
EXT=cpp
NAME	=nelo.so
LUNATICPLATYPUSDIR=../../GLnewin
OPTIM= -march=native -O2 -fno-rtti #-fno-exceptions
INCLUDE=-I$(LUNATICPLATYPUSDIR)/ -I$(LUNATICPLATYPUSDIR)/imgui/ -I$(LUNATICPLATYPUSDIR)/glm
CXXFLAGS= -Wconversion -Wall -Wextra -W -std=c++11 -fPIC $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
#LIBS= libLunaticPlatypus.a $(LUNATICPLATYPUSDIR)/libs/linux/libAlembic.a /usr/lib/libHalf.a /usr/lib/libIex.a
#pkg-config --static --libs glfw3
#ifeq ($(OS),Linux)
#    LIBS+=-lGL -ldl -pthread -lrt -lm -lXrandr -lXinerama -lXi -lXcursor -lXrender -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lX11 -lXxf86vm
#endif
#ifeq ($(OS),Darwin)
#    LIBS+= -framework openGL -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo
#endif
LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS)
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)
INSTALL = ../demo

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER) -shared $(OBJS) -o $(NAME) $(LDFLAGS)

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
