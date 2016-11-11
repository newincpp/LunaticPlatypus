SRC	= Camera.cpp \
	   FrameBuffer.cpp \
	   Importer.cpp \
	   Mesh.cpp \
	   OglCore.cpp \
	   RenderTexture.cpp \
	   shader.cpp \
	   Uniform.cpp \
	   main.cpp

EXT=cpp
NAME	=ogl
DEFINE 	= -DGLEW_STATIC 
DEBUG 	= -g 
GLM_DEFINE=-DGLM_SWIZZLE -DGLM_FORCE_AVX
OPTIM= $(GLM_DEFINE) -march=native -O2 -fno-rtti -fno-exceptions
INCLUDE=-I./../Obake/external -I./../Obake/external/EASTL -I./../Obake
CXXFLAGS= -Wall -Wextra -W -std=c++11 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
LIBS= -lGL -lglfw  ./libs/libGLEW.a ./libs/libassimp.a ./libs/libz.a
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
