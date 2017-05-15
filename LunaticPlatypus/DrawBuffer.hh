#pragma once

#include <vector>
#include <list>
#include <functional>
#include "Camera.hh"
#include "Mesh.hh"
#include "Light.hh"
#include "shader.hh"
//#include "fileWatcher.hh"

#include <iostream>

class DrawBuffer {
    public:
	bool _valid;
	std::vector<FrameBuffer> _fb;
	std::vector<Camera> _cameras;
	std::vector<Light> _lights;
	std::list<std::pair<Shader, std::vector<Mesh>>> _drawList;
	Shader _dynamicShadowCaster;
	unsigned int _activeCamera;
	DrawBuffer();
	void update(unsigned int currentFrame = 0);
	void render();
	void castDynamicShadows();
	void bindGBuffer(unsigned int camera_);
	void reset();
	void addAllUniformsToShaders() { addMeshUniformsToShaders(); addCameraUniformsToShaders(); }
	void addMeshUniformsToShaders();
	void addCameraUniformsToShaders();
};
