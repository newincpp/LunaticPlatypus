#include <iostream>
#include <algorithm>
#include <thread>
#include "glew.h"
#ifdef IMGUIENABLED
#include "imgui/imgui.h"
#endif
#include "WindowHandle.hh"

bool WindowHandle::_shouldClose;
GLFWwindow* WindowHandle::_lastUsedWindow; // to make static callback work
std::list<std::string> WindowHandle::_pressed;
WindowHandle::WindowHandle() : _window(nullptr) {
    if (!glfwInit()) {
	std::cout << "failed to init glfw3\n";
    }
    _shouldClose = false;
    _pressed.clear();
}

void WindowHandle::init() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
    _window = glfwCreateWindow(1920, 1080, "OpenGL", nullptr, nullptr); // Windowed

    if (!_window) {
        std::cout << "FAILED TO OPEN WINDOW\n";
        _shouldClose = true;
        return;
    } else {
        makeContextCurrent(_window);
    }

    glewExperimental=true;
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        std::cout << "Error: " << glewGetErrorString(err) << '\n';
        _shouldClose = true;
        return;
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
    if (_window) {
        glfwSetKeyCallback(_window, WindowHandle::keyCallback);
        glfwSetCursorPosCallback(_window, WindowHandle::mouseCallback);
    }
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

void WindowHandle::keyCallback(GLFWwindow* w_, int key_, int, int keyStatus_, int) {
#ifdef IMGUIENABLED
    ImGui_ImplGlfwGL3_KeyCallback(w_, key_, scanCode_, keyStatus_, modsKey_);
#endif
    if ((keyStatus_ == 1 && key_ == 256) || glfwWindowShouldClose(w_)) {
	_shouldClose = true; // TODO need to be replaced by en event
    }

    std::string k = std::to_string(key_);
    std::list<std::string>::iterator i = std::find(_pressed.begin(), _pressed.end(), k);
    if (keyStatus_ == GLFW_PRESS) {
        if (i == _pressed.end()) {
            _pressed.push_back(k);
        }
    } else if (keyStatus_ == GLFW_RELEASE) {
        if (i != _pressed.end()) {
            _pressed.erase(i);
        }
    }
    EventInterface::sExec("keyboard");
}

void WindowHandle::mouseCallback(GLFWwindow* _window, double, double) {
    _lastUsedWindow = _window;
    EventInterface::sExec("mouse");
}

glm::vec2 WindowHandle::getMousePos() {
    double xpos;
    double ypos;
    glfwGetCursorPos(_lastUsedWindow, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}
glm::vec2 WindowHandle::getMouseMovement() {
    static glm::vec2 oldPos(-1.0f, -1.0f);
    if (oldPos.x <= 0.0f && oldPos.y <= 0.0f) {
        oldPos = getMousePos();
        return glm::vec2(0.0f, 0.0f);
    } else {
        glm::vec2 newp = getMousePos();
        glm::vec2 v = oldPos - newp;
        oldPos = newp;
        return v;
    }
}
