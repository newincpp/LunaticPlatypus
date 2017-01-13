#include "PlatypusHeart.hh"
#include "imgui/imgui.h"

WindowHandle::WindowHandle() {
    glewExperimental = true;
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    _window = glfwCreateWindow(1920, 1024, "OpenGL", nullptr, nullptr); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), nullptr); // Fullscreen
    glfwMakeContextCurrent(_window);
    glewExperimental=true;
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
    std::cout << "glfw initialized" << std::endl;
}

bool WindowHandle::exec() {
    glfwSwapBuffers(_window);
    glfwPollEvents();
    if (glfwGetKey(_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	glfwSetWindowShouldClose(_window, GL_TRUE);
    return !glfwWindowShouldClose(_window);
}

WindowHandle::~WindowHandle() {
    glfwTerminate();
}


Heart::Heart() {
    _renderer.getDrawBuffer().reset(_game.sceneFileName);
}

void Heart::run() {
    bool mod = false;
    char imguistr[512] = "";
    while (_win.exec()) {
	ImGui::NewFrame();
	_renderer.render();
	_game.update();

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::InputText("scene: ", imguistr,  512)) {
	    _game.sceneFileName = imguistr;
	    mod = true;
	}
	if (ImGui::IsItemActivePreviousFrame() && !ImGui::IsItemActive() && ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Enter]) && mod) {
	    std::cout << "new Scene:" << _game.sceneFileName << '\n';
	    if (_fw) {
	        delete _fw;
	    }
	    _fw = new FileWatcher(_game.sceneFileName.c_str());
	    mod = false;
	} 
	if (_fw) {
	    if (_fw->isModified()) {
		_renderer.getDrawBuffer().reset(_game.sceneFileName);
		std::cout << "mod!\n";
	    }
	}
	_scene.update();
	ImGui::Render();
    }
}

int main()
{
    Heart p;
    p.run();
}
