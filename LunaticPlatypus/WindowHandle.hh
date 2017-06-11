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
	    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
	    GLFWwindow* threadedWin = glfwCreateWindow(600, 600, "OpenGL", nullptr, _window); // Windowed
	    glfwMakeContextCurrent(threadedWin);
	    glewExperimental=true;
	    GLenum err = glewInit();
	    glGetError();
	    if (GLEW_OK != err) {
		std::cout << "Error: " << glewGetErrorString(err) << '\n';
	    }
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
