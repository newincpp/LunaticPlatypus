#pragma once
#include <vector>
#include "RenderTexture.hh"

class FrameBuffer {
    private:
	GLuint _fbo;
    public:
	std::vector<RenderTexture<GL_RGB>> _rtt;
	RenderTexture<GL_DEPTH_COMPONENT> _rttDepth;

    	FrameBuffer();
	void enable();
	void addBuffer(std::string&&, glm::vec2 = glm::vec2(1920, 1080));
	void addDepthBuffer(std::string&&, glm::vec2 = glm::vec2(1920, 1080));
	void bindGBuffer();
	void disable();
	~FrameBuffer();
};
