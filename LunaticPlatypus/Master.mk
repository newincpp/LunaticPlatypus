OS	:= $(shell uname -s)

IMPORTER=ALEMBIC
DEFINE 	= -DGLEW_STATIC #-DIMGUIENABLED
DEBUG 	= -g
GLM_DEFINE=-DGLM_SWIZZLE #-DGLM_FORCE_AVX
OPTIM= $(GLM_DEFINE) -march=native -O0 -fno-rtti #-fno-exceptions
INCLUDE= -I./ # -I./imgui
CXXFLAGS= -Wall -Wextra -W -std=c++11 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)
IMGUIENABLED=false

LDFLAGS	=  $(OPTIM) $(DEBUG) $(LIBS)


CXX	= clang++
LINKER	= $(CXX)
EXT=cpp
