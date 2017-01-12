#include "DrawBuffer.hh"
#include "Importer.hh"
#include <imgui.h>

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

DrawBuffer::DrawBuffer() :_sceneName(STRINGIZE(DEFAULT_SCENE)), _mod(false), _fw(nullptr), _activeCamera(1) {
    _fb.reserve(8);
    _cameras.reserve(512);
}

void DrawBuffer::update() {
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::InputText("scene: ", _sceneName, 256)) {
	_mod = true;
    }
    if (ImGui::IsItemActivePreviousFrame() && !ImGui::IsItemActive() && ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Enter]) && _mod) {
	reset();
	_fw = new FileWatcher(_sceneName);
	_mod = false;
    } 

    if (_fw) {
        if (_fw->isModified()) {
	    reset();
            std::cout << "mod!\n";
        }
    }
}

void DrawBuffer::render() {
    ImGui::Text("camera in use: %d", _activeCamera);
    for (unsigned int a = 0; a < _activeCamera && a < _cameras.size(); ++a) {
	_cameras[a].use();
	for (Mesh& m : _meshes) {
	    m.render();
	}
	_cameras[a].unUse();
    }
}

void DrawBuffer::bindGBuffer(unsigned int camera_) {
    _cameras[camera_].bindFramebuffer();
}

void DrawBuffer::reset() {
    _meshes.clear();
    _cameras.clear();
    Importer iscene(_sceneName, *this);
}
