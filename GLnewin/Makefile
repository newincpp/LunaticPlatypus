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
	   main.cpp

SRC+=$(IMGUI)

EXT=cpp
NAME	=ogl
DEFINE 	= -DGLEW_STATIC -DTINYOBJLOADER
#DEBUG 	= -g
GLM_DEFINE=-DGLM_SWIZZLE -DGLM_FORCE_AVX
OPTIM= $(GLM_DEFINE) -march=native -O2 -fno-rtti -fno-exceptions
INCLUDE= -I./imgui -I./Alembic/OpenEXR -I./
CXXFLAGS= -Wall -Wextra -W -std=c++14 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
LIBS= -lGL -lglfw  ./libs/libGLEW.a ./libs/libassimp.a ./libs/libz.a ./libs/libAlembic.a ./libs/libHalf.a 
LDFLAGS	= $(LIBS) $(OPTIM) $(DEBUG) $(LIBS)
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
