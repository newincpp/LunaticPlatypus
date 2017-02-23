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
	decltype(DrawBuffer::_shaders)::iterator _sgBuffer;
	decltype(DrawBuffer::_shaders)::iterator _sPostProc;
	//Shader *_sgBuffer = nullptr;
	//Shader *_sPostProc = nullptr;
	DrawBuffer _s;
	Mesh _renderTarget;
	GLuint fractalTex;
	unsigned int _currentFrame = 0;
    public:
	OglCore();
	inline DrawBuffer& getDrawBuffer() { return _s; }
	void init();
	void renderScene();
	void render();
};
