#include "PlatypusHeart.hh"
#include "imgui/imgui.h"
#include <cassert>
#include <chrono>

Heart::Heart() {
    if (_game == nullptr) {
	std::cout << "SetGameClass() macro has not been called I will now assert\n";
    }
    assert(_game != nullptr);
    _game->_lunaticPlatypus = this;
    if (_game->_scene.empty()) {
	_game->_scene = STRINGIZE(DEFAULT_SCENE);
    }
    std::cout << "default scene: " << _game->_scene << "\n";
    _renderer.getDrawBuffer().reset(_game->_scene);
}

void Heart::run() {
    bool mod = false;
    char imguistr[512] = "";
    std::chrono::time_point<std::chrono::high_resolution_clock> endFrame;
    std::chrono::time_point<std::chrono::high_resolution_clock> beginFrame;

    while (_win.exec()) {
	ImGui::NewFrame();
	endFrame = std::chrono::high_resolution_clock::now();
	ImGui::Text("\nApplication average %ld ms/frame (%.1f FPS)", std::chrono::duration_cast<std::chrono::milliseconds>(endFrame-beginFrame).count(), 1000.0/double(std::chrono::duration_cast<std::chrono::milliseconds>(endFrame-beginFrame).count()));
	beginFrame = std::chrono::high_resolution_clock::now();

	_renderer.render();
	_game->update();

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::InputText("scene: ", imguistr,  512)) {
	    _game->_scene = imguistr;
	    mod = true;
	}
	if (ImGui::IsItemActivePreviousFrame() && !ImGui::IsItemActive() && ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Enter]) && mod) {
	    std::cout << "new Scene:" << _game->_scene << '\n';
	    if (_fw) {
	        delete _fw;
	    }
	    _fw = new FileWatcher(_game->_scene.c_str());
	    mod = false;
	} 
	if (_fw) {
	    if (_fw->isModified()) {
		_renderer.getDrawBuffer().reset(_game->_scene);
		std::cout << "mod!\n";
	    }
	}
	_scene.update();
	ImGui::Render();
    }
}

void Heart::loadScene() {
    _renderer.getDrawBuffer().reset(_game->_scene);
}

int main() {
    Heart p;
    p.run();
}
