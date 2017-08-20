#pragma once
#include "GLFW/glfw3.h"
#include "EventCore.hh"
#include <glm/glm.hpp>
#include <map>

class WindowHandle {
    private:
	GLFWwindow* _window;
        static GLFWwindow* _lastUsedWindow; // to make static callback work
	EventInterface _e;
	static void keyCallback(GLFWwindow*, int, int, int, int);
	static void mouseCallback(GLFWwindow *, double, double);
	static bool _shouldClose;
        static std::list<std::string> _pressed;
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
            for (std::string& p : _pressed) {
                std::cout << "yay: " << p << '\n';
                EventInterface::sExec(p);
            }
	}
        static glm::vec2 getMousePos();
        static glm::vec2 getMouseMovement();

	~WindowHandle();
};
