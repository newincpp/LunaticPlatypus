#include <OglCore.hh>

#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

#include <imgui.h>

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
	    }
	    std::cout << "Vendor: " << glGetString(GL_VENDOR) << '\n';
	    std::cout << "GL Version: " << glGetString(GL_VERSION) << '\n';
	    std::cout << "Renderer: " << glGetString(GL_RENDERER) << '\n';
	    std::cout << "Shader Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';


	    ImGuiIO& io = ImGui::GetIO();
	    int w, h;
	    int display_w, display_h;
	    glfwGetWindowSize(_window, &w, &h);
	    glfwGetFramebufferSize(_window, &display_w, &display_h);
	    io.DisplaySize = ImVec2((float)w, (float)h);
	    io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);
	    io.RenderDrawListsFn = ImGui_RenderDrawLists;
	    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;                         // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
	    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
	    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
	    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
	    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
	    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
	    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
	    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
	    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
	    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
	    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
	    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
	    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
	    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
	    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
	    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
	    unsigned char* pixels;
	    int width, height;
	    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
	    glfwSetMouseButtonCallback(_window, ImGui_ImplGlfwGL3_MouseButtonCallback);
	    glfwSetKeyCallback(_window, ImGui_ImplGlfwGL3_KeyCallback);
	    glfwSetCharCallback(_window, ImGui_ImplGlfwGL3_CharCallback);


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
