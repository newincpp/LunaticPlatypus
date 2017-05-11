#include "PlatypusHeart.hh"
#include "imgui/imgui.h"
#include <cassert>
#include <chrono>

#include "Importer.hh"

Heart::Heart() : _fw(nullptr) {
    if (_game == nullptr) {
	std::cout << "SetGameClass() macro has not been called I will now assert\n";
    }
    assert(_game != nullptr);
    _game->_lunaticPlatypus = this;
    if (_game->_scene.empty()) {
	_game->_scene = STRINGIZE(DEFAULT_SCENE);
    }
    std::cout << "default scene: " << _game->_scene << "\n";
    loadScene();
    _game->postEngineInit();
}

void Heart::run() {
    bool mod = false;
    char imguistr[512] = "";
    std::chrono::time_point<std::chrono::high_resolution_clock> endFrame;
    std::chrono::time_point<std::chrono::high_resolution_clock> beginFrame;

    while (_win.exec()) {
	ImGui::NewFrame();
	endFrame = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endFrame-beginFrame).count();
	ImGui::Text("\nApplication average %f ms/frame (%.1f FPS)", deltaTime, 1000.0/double(std::chrono::duration_cast<std::chrono::milliseconds>(endFrame-beginFrame).count()));
	beginFrame = std::chrono::high_resolution_clock::now();

	_renderer.render();
	for (GameClass& g: _game->_gameClasses) {
	    g.update(deltaTime);
	}
	_game->update();

	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::InputText("scene: ", imguistr,  512)) {
	    _game->_scene = imguistr;
	    mod = true;
	}
	if (ImGui::IsItemActivePreviousFrame() && !ImGui::IsItemActive() && ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Enter]) && mod) {
	    std::cout << "new Scene:" << _game->_scene << '\n';
	    //_fw = _game->_scene;

	    if (_fw) {
	        delete _fw;
	    }
	    _fw = new FileWatcher(_game->_scene.c_str());
	    mod = false;
	}
	_scene.update();
	ImGui::Render();
    }
}

void Heart::loadScene() {
    if (_fw) {
	delete _fw;
    }
    _fw = new FileWatcher(_game->_scene.c_str());
    _renderer.getDrawBuffer().reset();
    Importer iscene(_game->_scene, _renderer.getDrawBuffer(), _game);
    _fw->callBack = [this]() { std::cout << "automatic file reloader temporarily deleted\n"; };
}

void Heart::IGamelogic::postEngineInit() {
    // default function do noting
}

int main() {
    Heart p;
    p.run();
}
