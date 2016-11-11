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
	    getGlError(__FILE__, __LINE__);

	    _window = glfwCreateWindow(1920, 1024, "OpenGL", nullptr, nullptr); // Windowed
	    getGlError(__FILE__, __LINE__);
	    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
	    glfwMakeContextCurrent(_window);
	    getGlError(__FILE__, __LINE__);
	    //glewExperimental = GL_TRUE;
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
	    getGlError(__FILE__, __LINE__);
	}
	void loop() {

	    while(!glfwWindowShouldClose(_window))
	    {
		std::chrono::time_point<std::chrono::high_resolution_clock> begin = std::chrono::high_resolution_clock::now();
		long rendertime = _c.render();

		glfwSwapBuffers(_window);
		glfwPollEvents();
		if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		    glfwSetWindowShouldClose(_window, GL_TRUE);
		std::chrono::time_point<std::chrono::high_resolution_clock> end = std::chrono::high_resolution_clock::now();
		//std::cout << "GPU: " << rendertime << "ns\t\t" << 1000000000.0/double(rendertime) << "fps (theorical)\n"; 
		//std::cout << "CPU: " << std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count() << "ns\t\t" << 1000000000.0/double(std::chrono::duration_cast<std::chrono::nanoseconds>(end-begin).count()) << "fps\n\n"; 
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
