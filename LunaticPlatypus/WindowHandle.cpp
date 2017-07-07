#include <iostream>
#include <thread>
#include "glew.h"
#ifdef IMGUIENABLED
#include "imgui/imgui.h"
#endif
#include "WindowHandle.hh"

bool WindowHandle::_shouldClose;
WindowHandle::WindowHandle() {
    if (!glfwInit()) {
	std::cout << "failed to init glfw3\n";
    }
    _shouldClose = false;
}

void WindowHandle::init() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    _window = glfwCreateWindow(1920, 1080, "OpenGL", nullptr, nullptr); // Windowed

    makeContextCurrent(_window);

    glewExperimental=true;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << '\n';
    }
#ifdef IMGUIENABLED
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
    //glfwSetKeyCallback(_window, ImGui_ImplGlfwGL3_KeyCallback);
    glfwSetCharCallback(_window, ImGui_ImplGlfwGL3_CharCallback);
#endif
    glfwSetKeyCallback(_window, WindowHandle::keyCallback);
    std::cout << "glfw initialized" << std::endl;
}

bool WindowHandle::exec() {
    if (!_window) {
	return true; // ugly spinlock
    }
    return !_shouldClose;
}

WindowHandle::~WindowHandle() {
    glfwTerminate();
}

void WindowHandle::keyCallback(GLFWwindow* w_, int key_, int scanCode_, int keyStatus_, int modsKey_) {
#ifdef IMGUIENABLED
    ImGui_ImplGlfwGL3_KeyCallback(w_, key_, scanCode_, keyStatus_, modsKey_);
#endif
    if ((keyStatus_ == 1 && key_ == 256) || glfwWindowShouldClose(w_)) {
	_shouldClose = true; // TODO need to be replaced by en event
    }
    EventInterface::sExec(std::to_string(key_));
    EventInterface::sExec("keyboard");
}
