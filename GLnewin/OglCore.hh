#pragma once
#include <vector>
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
	Uniform _uPostPRocessTexture;
	Shader *_sgBuffer = nullptr;
	Shader *_sPostProc = nullptr;
	DrawBuffer _s;
	Mesh _renderTarget;
	GLuint fractalTex;
    public:
	OglCore();
	inline DrawBuffer& getDrawBuffer() { return _s; }
	void init();
	void renderScene();
	void render();
};
