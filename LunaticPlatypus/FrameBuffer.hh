#pragma once
#include <vector>
#include "RenderTexture.hh"

class FrameBuffer {
    private:
	GLuint _fbo;
    public:
	std::vector<RenderTexture<COLOR>> _rtt;
	RenderTexture<DEPTH> _rttDepth;

    	FrameBuffer();
	void enable();
	void addBuffer(std::string&&, glm::vec2 = glm::vec2(1920, 1080));
	void addDepthBuffer(std::string&&, glm::vec2 = glm::vec2(1920, 1080));
	void bindGBuffer();
	void disable();
	~FrameBuffer();
};
