#pragma once
#include <list>
#include <iostream>
#include <utility>
#include <chrono>
#include "shader.hh"
#include "glew.h"
#include "Uniform.hh"
#include "DrawBuffer.hh"

#define checkGlError getGlError(__FILE__, __LINE__);
void getGlError(const char* file_, unsigned long line_);

class OglCore {
    private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _beginTime;
	Uniform uTime;
	//decltype(DrawBuffer::_drawList)::iterator _sgBuffer;
	Shader _compositor;
	DrawBuffer _s;
	Mesh _renderTarget;
	//GLuint _illuminationBuffer;
	unsigned int _currentFrame = 0;
    public:
	OglCore();
	inline Camera& getCamera(unsigned long i = 0) { return getDrawBuffer()._cameras[i]; }
	inline DrawBuffer& getDrawBuffer() { return _s; }
	void init();
	void renderScene();
	void render();
};
