#include "Scene.hh"
#include "Importer.hh"
#include <imgui.h>

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
Scene::Scene() :_sceneName(STRINGIZE(DEFAULT_SCENE)), _mod(false), _activeCamera(1) {
    _cameras.reserve(512);
    std::cout << _sceneName << '\n';
}

void Scene::update() {
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::InputText("scene: ", _sceneName, 256)) {
	_mod = true;
    }
    if (ImGui::IsItemActivePreviousFrame() && !ImGui::IsItemActive() && ImGui::IsKeyPressed(io.KeyMap[ImGuiKey_Enter]) && _mod) {
	_meshes.clear();
	_cameras.clear();
	Importer iscene(_sceneName, *this);
	_mod = false;
    } 
}

void Scene::render() {
    ImGui::Text("camera in use: %d", _activeCamera);
    for (unsigned int a = 0; a < _activeCamera && a < _cameras.size(); ++a) {
	_cameras[a].use();
	for (Mesh& m : _meshes) {
	    m.render();
	}
	_cameras[a].unUse();
    }
}

void Scene::bindGBuffer(unsigned int camera_) {
    _cameras[camera_].bindFramebuffer();
}
