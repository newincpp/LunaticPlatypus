include Master.mk

ifeq ($(IMGUIENABLED), true)
    IMGUI	= imgui/imgui.cpp \
    	  	imgui/imgui_demo.cpp \
    	  	imgui/imgui_draw.cpp \
    	  	imgui/imguiImplementation.cpp
endif
    
SRC	= Camera.cpp \
	   FrameBuffer.cpp \
	   Importer.cpp \
	   Mesh.cpp \
	   Light.cpp \
	   OglCore.cpp \
	   RenderTexture.cpp \
	   Texture.cpp \
	   ImageRAM.cpp \
	   shader.cpp \
	   Uniform.cpp \
	   DrawBuffer.cpp \
	   Node.cpp \
	   WindowHandle.cpp \
	   PlatypusHeart.cpp \
	   EventCore.cpp \
	   DynamicGameClass.cpp \
	   Node.cpp \
	   RenderThread.cpp \
	   fileWatcher.cpp

ifeq ($(IMGUIENABLED), true)
	SRC+=$(IMGUI)
endif

NAME	=LunaticPlatypus

ifeq ($(OS),Linux)
    LIBFOLDER=linux
endif
ifeq ($(OS),Darwin)
    LIBFOLDER=OSX
endif
LIBS:= ./libs/$(LIBFOLDER)/Objectsoup/libGLEW/*.o \
    ./libs/$(LIBFOLDER)/Objectsoup/libglfw3/*.o
    
INSTALL = ../demo


ifeq ($(IMPORTER),GLTF2)
    DEFINE+=-DGLTF2
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

OBJS	= $(SRC:.$(EXT)=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(RM) lib$(NAME).a
	ar -cvq lib$(NAME).a $(OBJS) $(LIBS) > /dev/null
	ranlib lib$(NAME).a

extractLibs:
	cd libs/ ; ./extractObject.sh $(LIBFOLDER)

install:
	cp lib$(NAME).a $(INSTALL)
	#cp project.mk $(INSTALL)/Makefile

project:
	make -C $(INSTALL) -$(MAKEFLAGS) -B

full: extractLibs $(NAME) install project

clean:
	$(RM) $(OBJS) *.swp *~ *#

fclean: clean
	$(RM) $(NAME)

re: make -B -j4

.PHONY: all clean fclean re
