IMGUI	= imgui/imgui.cpp \
	  imgui/imgui_demo.cpp \
	  imgui/imgui_draw.cpp \
	  imgui/imguiImplementation.cpp

SRC	= Camera.cpp \
	   FrameBuffer.cpp \
	   Importer.cpp \
	   Mesh.cpp \
	   OglCore.cpp \
	   RenderTexture.cpp \
	   Texture.cpp \
	   shader.cpp \
	   Uniform.cpp \
	   DrawBuffer.cpp \
	   node.cpp \
	   fileWatcher.cpp \

SRC+=$(IMGUI)

EXT=cpp
NAME	=LunaticPlatipus
#DEFINE 	= -DDEFAULT_SCENE=nelo.obj -DGLEW_STATIC -DALEMBIC 
DEFINE 	= -DDEFAULT_SCENE=nelo.obj -DGLEW_STATIC -DTINYOBJLOADER
#DEFINE 	= -DDEFAULT_SCENE=nelo.dae -DGLEW_STATIC
DEBUG 	= -g
GLM_DEFINE=-DGLM_SWIZZLE -DGLM_FORCE_AVX
OPTIM= $(GLM_DEFINE) -march=native -O2 -fno-rtti #-fno-exceptions
INCLUDE= -I./imgui -I./Alembic/OpenEXR -I./ -I./Alembic
CXXFLAGS= -Wall -Wextra -W -std=c++14 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
#LIBS= ./libs/libGLEW.a ./libs/libassimp.a ./libs/libAlembic.a ./libs/libHalf.a ./libs/libhdf5.a ./libs/libz.a 
LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS)
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)
INSTALL = ../demo

all: $(NAME)

$(NAME): $(OBJS)
	rm lib$(NAME).a
	ar -cvq lib$(NAME).a $(OBJS) ./libs/Objectsoup/*.o > /dev/null

#$(LINKER)  $(OBJS) -o $(NAME) $(LDFLAGS)

extractLibs:
	cd libs ; ls ; ./extractObject.sh

install:
	cp lib$(NAME).a $(INSTALL)
	cp Makefile.base $(INSTALL)/Makefile

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
