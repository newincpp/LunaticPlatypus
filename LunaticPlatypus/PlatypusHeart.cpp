#include "PlatypusHeart.hh"
#include "imgui/imgui.h"
#include <cassert>
#include <chrono>

#include "Importer.hh"

Graph* Heart::_scene = nullptr;
Heart::Heart() : _fw(nullptr), _win() {
    if (_game == nullptr) {
	std::cout << "SetGameClass() macro has not been called I will now assert\n";
    }
    assert(_game != nullptr);
    _game->_lunaticPlatypus = this;
    _scene = new Graph();
    if (_game->_scene.empty()) {
	_game->_scene = STRINGIZE(DEFAULT_SCENE);
    }
    std::cout << "default scene: " << _game->_scene << "\n";
    _renderThread.uniqueTasks.push_back([this]() {
	    _win.init();
	    //_win.makeContextCurrent();
	_renderThread.unsafeGetRenderer().init();
        std::cout << "renderer initialised\n";
	    });
    _renderThread.unsafeGetRenderer().swap = [this]() {
	//std::cout << "thread: " << std::this_thread::get_id() << '\n';
	_win.pollEvents();
        _win.swapBuffer();
    };
    _renderThread.run();
    loadScene();

    std::this_thread::sleep_for(std::chrono::milliseconds(33)); // wait for 1 frame before the opengl init to avoid mutax architecture
    _game->postEngineInit();
}

void Heart::run() {
    bool mod = false;
    char imguistr[512] = "";
    std::chrono::time_point<std::chrono::high_resolution_clock> endFrame;
    std::chrono::time_point<std::chrono::high_resolution_clock> beginFrame;

    while (_win.exec()) {
	endFrame = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(endFrame-beginFrame).count();
	beginFrame = std::chrono::high_resolution_clock::now();

	for (GameClass& g: _game->_gameClasses) {
	    g.update(deltaTime/1000);
	}
	_game->update();

#ifdef IMGUIENABLED
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
#endif
	//_scene->update();
    }
    _renderThread.setKeepAlive(false);
}

void Heart::loadScene() {
    if (_fw) {
	delete _fw;
    }
    _fw = new FileWatcher(_game->_scene.c_str());
    _renderThread.unsafeGetRenderer().getDrawBuffer().reset();
    // should take _renderThread in parameter and add mesh via uniquetask
    Importer iscene(_game->_scene, _renderThread, _game, *_scene);
    _fw->callBack = [this]() { std::cout << "automatic file reloader temporarily deleted\n"; };
}

void Heart::addScene(std::string filename) {
    Importer iscene(filename, _renderThread, _game, *_scene);
}

void Heart::IGamelogic::postEngineInit() {
    // default function do noting
}

int main() {
    Heart p;
    p.run();
}
