#include "OglCore.hh"

#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

class MinimalWindow {
    public:
	GLFWwindow* _window;
	OglCore& _c;
	MinimalWindow(OglCore& c_) : _c(c_) {
	    glfwInit();
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
	    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	    _window = glfwCreateWindow(1920, 1024, "OpenGL", nullptr, nullptr); // Windowed
	    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
	    glfwMakeContextCurrent(_window);
	    GLenum err = glewInit();
	    glGetError();
	    if (GLEW_OK != err) {
		std::cout << "Error: " << glewGetErrorString(err) << '\n';
	    } else {
		std::cout << "glew is ok =D\n";
	    }
	    std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	    std::cout << "Shader Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	    _c.init();
	}
	void loop() {

	    while(!glfwWindowShouldClose(_window))
	    {
		_c.render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		    glfwSetWindowShouldClose(_window, GL_TRUE);
	    }
	}
	~MinimalWindow() {
	    glfwTerminate();
	}
};

int main()
{
    OglCore c;
    MinimalWindow w(c);
    w.loop();
}
