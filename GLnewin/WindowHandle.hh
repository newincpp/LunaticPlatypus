#include "GLFW/glfw3.h"

class WindowHandle {
    public:
	GLFWwindow* _window;
	WindowHandle();
	bool exec();
	~WindowHandle();
};
