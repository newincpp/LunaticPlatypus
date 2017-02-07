#include "GLFW/glfw3.h"
#include "EventCore.hh"
#include <map>

class WindowHandle {
    private:
	GLFWwindow* _window;
	EventInterface _e;
	static void keyCallback(GLFWwindow*, int, int, int, int);
    public:
	WindowHandle();
	bool exec();
	~WindowHandle();
};
