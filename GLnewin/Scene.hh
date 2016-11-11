#pragma once

#include <vector>
#include "Camera.hh"
#include "Mesh.hh"

class Scene {
    public:
	std::vector<Mesh> _meshes;
	std::vector<Camera> _cameras;
	unsigned int _activeCamera;
	Scene() : _activeCamera(1) {
	    _cameras.reserve(32);
	}
	void render() {
	    for (unsigned int a = 0; a < _activeCamera; ++a) {
		_cameras[a].use();
		for (Mesh& m : _meshes) {
		    m.render();
		}
		_cameras[a].unUse();
	    }
	}
	void bindGBuffer(unsigned int camera_) {
	    _cameras[camera_].bindFramebuffer();
	}
};
