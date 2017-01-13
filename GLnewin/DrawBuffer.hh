#pragma once

#include <vector>
#include "Camera.hh"
#include "Mesh.hh"
#include "fileWatcher.hh"

#include <iostream>

class DrawBuffer {
    public:
	std::vector<FrameBuffer> _fb;
	std::vector<Mesh> _meshes;
	std::vector<Camera> _cameras;
	unsigned int _activeCamera;
	DrawBuffer() ;
	void update();
	void render();
	void bindGBuffer(unsigned int camera_);
	void reset(std::string&);
};
