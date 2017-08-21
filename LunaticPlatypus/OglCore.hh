#pragma once
#include <list>
#include <functional>
#include <iostream>
#include <utility>
#include <chrono>
#include "shader.hh"
#include "glew.h"
#include "Uniform.hh"
#include "DrawBuffer.hh"
#include "ImageRAM.hh"

#define checkGlError getGlError(__FILE__, __LINE__);
void getGlError(const char* file_, unsigned long line_);

class OglCore {
    private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _beginTime;
	Uniform uTime;
	Shader _compositor;
	DrawBuffer _s;
	Mesh _renderTarget;
	unsigned int _currentFrame = 0;
        std::list<ImageRAM> _randomAccessBuffers;
    public:
	OglCore();
	inline Camera& getCamera(unsigned long i = 0) { return getDrawBuffer()._cameras[i]; }
	inline DrawBuffer& getDrawBuffer() { return _s; }
        inline std::list<ImageRAM>& getBuffers() { return _randomAccessBuffers; }
	void init();
	void renderScene();
	void render();
	std::function<void(void)> swap;
};
