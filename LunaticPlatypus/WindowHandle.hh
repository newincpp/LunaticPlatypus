#pragma once
#include "GLFW/glfw3.h"
#include "EventCore.hh"
#include <map>

class WindowHandle {
    private:
	//GLFWwindow* _windowThread;
	EventInterface _e;
	static void keyCallback(GLFWwindow*, int, int, int, int);
    public:
	GLFWwindow* _window;
	WindowHandle();
	void init();
	bool exec();
	inline void makeContextCurrent(GLFWwindow* win) {
	    glfwMakeContextCurrent(win);
	    //glGetError();
	    std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	    std::cout << "Shader Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	}
	inline void swapBuffer() {
	    glfwSwapBuffers(_window);
	}
	~WindowHandle();
};
