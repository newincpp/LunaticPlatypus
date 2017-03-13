#pragma once

#include <vector>
#include <list>
#include <functional>
#include "Camera.hh"
#include "Mesh.hh"
#include "shader.hh"
//#include "fileWatcher.hh"

#include <iostream>

class DrawBuffer {
    public:
	bool _valid;
	std::vector<FrameBuffer> _fb;
	std::vector<Camera> _cameras;
	//std::vector<Mesh> _meshes;
	std::list<std::pair<Shader, std::vector<Mesh>>> _drawList;
	unsigned int _activeCamera;
	DrawBuffer();
	void update(unsigned int currentFrame = 0);
	void render();
	void bindGBuffer(unsigned int camera_);
	void reset(std::string&);
	void addAllUniformsToShaders() { addMeshUniformsToShaders(); addCameraUniformsToShaders(); }
	void addMeshUniformsToShaders();
	void addCameraUniformsToShaders();
};
