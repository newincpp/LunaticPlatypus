include $(LUNATICPLATYPUSDIR)/Master.mk
INCLUDE=-I$(LUNATICPLATYPUSDIR)/  -I$(LUNATICPLATYPUSDIR)/glm
ifeq ($(IMGUIENABLED), true)
	INCLUDE+=-I$(LUNATICPLATYPUSDIR)/imgui/
endif

CXXFLAGS= -Wconversion -Wall -Wextra -W -std=c++14 $(OPTIM) $(DEFINE) $(DEBUG) $(INCLUDE)

ifeq ($(IMPORTER),ALEMBIC)
	LIBS=$(LUNATICPLATYPUSDIR)/libs/linux/libAlembic.a #/usr/lib/libHalf.a /usr/lib/libIex.a
endif
LIBS+= libLunaticPlatypus.a 

#pkg-config --static --libs glfw3
ifeq ($(OS),Linux)
    LIBS+=-lGL -ldl -pthread -lrt -lm -lXrandr -lXinerama -lXi -lXcursor -lXrender -ldrm -lXdamage -lXfixes -lX11-xcb -lxcb-glx -lxcb-dri2 -lX11 -lXxf86vm
endif
ifeq ($(OS),Darwin)
    LIBS+= -framework openGL -framework Cocoa -framework IOKit -framework CoreFoundation -framework CoreVideo
endif
OBJS	= $(SRC:.$(EXT)=.o)
