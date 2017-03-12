#include "DrawBuffer.hh"
#include "Importer.hh"
#include <imgui.h>

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

DrawBuffer::DrawBuffer() : _activeCamera(1) {
    _fb.reserve(8);
    _cameras.reserve(512);
}

void DrawBuffer::update(unsigned int currentFrame) {
    for (unsigned int a = 0; a < _activeCamera && a < _cameras.size(); ++a) {
	_cameras[a].updateUniform(currentFrame);
    }
}

void DrawBuffer::render() {
    ImGui::Text("camera in use: %d", _activeCamera);
    for (unsigned int a = 0; a < _activeCamera && a < _cameras.size(); ++a) {
	_cameras[a].use();
	for (std::pair<Shader, std::vector<Mesh>>& material : _drawList) {
	    if (material.first.use()) {
		for (Mesh& m : material.second) {
		    m.render();
		}
	    } 
	}
	_cameras[a].unUse();
    }
}

void DrawBuffer::bindGBuffer(unsigned int camera_) {
    _cameras[camera_].bindFramebuffer();
}

void DrawBuffer::reset(std::string& scene_) {
    _cameras.clear();
    for (std::pair<Shader, std::vector<Mesh>>& material : _drawList) {
	material.second.clear();
    }
    _drawList.clear();
    Importer iscene(scene_, *this);
    addAllUniformsToShaders();
}

void DrawBuffer::addMeshUniformsToShaders() {
    for (std::pair<Shader, std::vector<Mesh>>& material : _drawList) {
	for (Mesh& mesh : material.second) {
	    material.first.containUniform(mesh.uMeshTransform);
	}
    }
}

void DrawBuffer::addCameraUniformsToShaders() {
    for (std::pair<Shader, std::vector<Mesh>>& material : _drawList) {
	for (decltype(_cameras)::value_type& camera : _cameras) {
	    material.first.containUniform(camera.uView);
	    material.first.containUniform(camera.uProjection);
	}
    }
}
