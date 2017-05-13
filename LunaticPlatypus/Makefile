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
	   ImageRAM.cpp \
	   shader.cpp \
	   Uniform.cpp \
	   DrawBuffer.cpp \
	   node.cpp \
	   WindowHandle.cpp \
	   PlatypusHeart.cpp \
	   EventCore.cpp \
	   DynamicGameClass.cpp \
	   Node.cpp \
	   fileWatcher.cpp

SRC+=$(IMGUI)

EXT=cpp
OS	:= $(shell uname -s)
ifeq ($(OS),Linux)
    LIBFOLDER=linux
endif
ifeq ($(OS),Darwin)
    LIBFOLDER=OSX
endif
NAME	=LunaticPlatypus
IMPORTER=ALEMBIC
DEFINE 	= -DDEFAULT_SCENE=nelo.obj -DGLEW_STATIC 
DEBUG 	= -g
GLM_DEFINE=-DGLM_SWIZZLE #-DGLM_FORCE_AVX
OPTIM= $(GLM_DEFINE) -march=native -O0 -fno-rtti #-fno-exceptions
INCLUDE= -I./imgui  -I./ 
CXXFLAGS= -Wall -Wextra -W -std=c++11 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
LIBS:= ./libs/$(LIBFOLDER)/Objectsoup/libGLEW/*.o \
    ./libs/$(LIBFOLDER)/Objectsoup/libglfw3/*.o
    
LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS)
OBJS	= $(SRC:.$(EXT)=.o)
RM	= rm -f
CXX	= clang++
LINKER	= $(CXX)
INSTALL = ../demo

ifeq ($(IMPORTER),TINYOBJLOADER)
    DEFINE+=-DTINYOBJLOADER
endif
ifeq ($(IMPORTER),ALEMBIC)
    DEFINE+=-DALEMBICV2
    LIBS+= ./libs/$(LIBFOLDER)/Objectsoup/libHalf/*.o \
	  ./libs/$(LIBFOLDER)/Objectsoup/libAlembic/*.o

    INCLUDE+=-I./Alembic/OpenEXR -I./Alembic
endif
ifeq ($(IMPORTER),ASSIMP)
    DEFINE+=-DASSIMP
    LIBS+= ./libs/$(LIBFOLDER)/Objectsoup/libassimp/*.o \
	   ./libs/$(LIBFOLDER)/Objectsoup/libz/*.o
endif

RUN_ARGS := $(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))


all: $(NAME)

$(NAME): $(OBJS)
	$(RM) lib$(NAME).a
	ar -cvq lib$(NAME).a $(OBJS) $(LIBS) > /dev/null
	ranlib lib$(NAME).a

extractLibs:
	cd libs/ ; ./extractObject.sh $(LIBFOLDER)

install:
	cp lib$(NAME).a $(INSTALL)
	cp Makefile.base $(INSTALL)/Makefile

project:
	make -C $(INSTALL) -$(MAKEFLAGS) -B

full: extractLibs $(NAME) install project

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re