#include "DrawBuffer.hh"
#include "Importer.hh"
#include <imgui.h>

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

DrawBuffer::DrawBuffer() : _activeCamera(1) {
    _fb.reserve(8);
    _cameras.reserve(512);
}

void DrawBuffer::update() {
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

void DrawBuffer::reset(std::string& scene_) {
    _meshes.clear();
    _cameras.clear();
    Importer iscene(scene_, *this);
}

void DrawBuffer::addMeshUniformsToShaders() {
    for (Mesh mesh : _meshes) {
	mesh.uMeshTransform.addItselfToShaders(_shaders);
    }
}

void DrawBuffer::addCameraUniformsToShaders() {
    for (decltype(_cameras)::value_type& camera : _cameras) {
	camera.uView.addItselfToShaders(_shaders);
	camera.uProjection.addItselfToShaders(_shaders);
    }
}
