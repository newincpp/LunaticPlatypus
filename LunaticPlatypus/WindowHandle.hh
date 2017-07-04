#pragma once
#include "GLFW/glfw3.h"
#include "EventCore.hh"
#include <map>

class WindowHandle {
    private:
	GLFWwindow* _window;
	EventInterface _e;
	static void keyCallback(GLFWwindow*, int, int, int, int);
	static bool _shouldClose;
    public:
	WindowHandle();
	void init();
	bool exec();
	inline void makeContextCurrent(GLFWwindow* win) {
	    glfwMakeContextCurrent(win);
	    std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	    std::cout << "Shader Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	}
	inline void swapBuffer() {
	    glfwSwapBuffers(_window);
	}
	inline void pollEvents() {
	    glfwPollEvents();
	}
	~WindowHandle();
};
