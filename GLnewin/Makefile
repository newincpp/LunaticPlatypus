SRC	=Window.cpp \
	 Mesh.cpp \
	 Shader.cpp \
	 Uniform.cpp \
	 Object.cpp \
	 Camera.cpp

EXT     =cpp
NAME	=GLnewin.a
CXXFLAGS= -Wall -Wextra -W -std=c++0x -I./ -DGLEW_STATIC
LDFLAGS	= ./libs/*.o # -lGL -lX11 -DGLEW_STATIC
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= ar

all: $(NAME)

$(NAME): $(OBJS)
	$(LINKER) -cvq $(NAME) $(OBJS) $(LDFLAGS)
	cp $(NAME) ../

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re

