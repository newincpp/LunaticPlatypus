#pragma once
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
	inline void makeContextCurrent() {
	    glfwMakeContextCurrent(_window);
	    GLenum err = glewInit();
	    glGetError();
	    if (GLEW_OK != err) {
		std::cout << "Error: " << glewGetErrorString(err) << '\n';
	    }
	}
	inline void swapBuffer() {
	    glfwSwapBuffers(_window);
	}
	~WindowHandle();
};
