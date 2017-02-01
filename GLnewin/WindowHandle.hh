#include "GLFW/glfw3.h"

class WindowHandle {
    private:
	GLFWwindow* _window;
    public:
	WindowHandle();
	bool exec();
	//typedef void(* GLFWkeyfun) (GLFWwindow *, int, int, int, int)
	static void keyCallback(GLFWwindow *, int, int, int, int);
	~WindowHandle();
};
