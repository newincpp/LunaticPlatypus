#pragma once

#include <vector>
#include <functional>
#include "Camera.hh"
#include "Mesh.hh"
#include "shader.hh"
//#include "fileWatcher.hh"

#include <iostream>

class DrawBuffer {
    public:
	std::vector<FrameBuffer> _fb;
	std::vector<Mesh> _meshes;
	std::vector<Camera> _cameras;
	std::vector<Shader> _shaders;
	unsigned int _activeCamera;
	DrawBuffer();
	void update();
	void render();
	void bindGBuffer(unsigned int camera_);
	void reset(std::string&);
	void addAllUniformsToShaders() { addMeshUniformsToShaders(); addCameraUniformsToShaders(); }
	void addMeshUniformsToShaders();
	void addCameraUniformsToShaders();
};
