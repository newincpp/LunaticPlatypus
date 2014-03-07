SRCDIR	=GLnewin
SRC	=$(SRCDIR)/main.cpp \
	 $(SRCDIR)/Window.cpp \
	 $(SRCDIR)/Mesh.cpp \
	 $(SRCDIR)/Shader.cpp \
	 $(SRCDIR)/Uniform.cpp \
	 $(SRCDIR)/Object.cpp \
	 $(SRCDIR)/Camera.cpp
EXT     =cpp
NAME	=gl
CXXFLAGS= -Wall -Wextra -W -std=c++0x -I./
LDFLAGS	=-lGL -lX11 -DGLEW_STATIC ./libs/libGLEW.a ./libs/libassimp.a
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER) $(OBJS) $(LDFLAGS) -o $(NAME)

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re

