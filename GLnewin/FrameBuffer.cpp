#include <iostream>

#include "FrameBuffer.hh"
#include "OglCore.hh"

FrameBuffer::FrameBuffer() {
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    _rtt.reserve(4);
}

void FrameBuffer::enable() {
    //glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    if (_rtt.size()) {
	GLuint attachments[_rtt.size()];
	for (unsigned short i = 0; i < _rtt.size(); ++i) {
	    attachments[i] = _rtt[i].getAttachment();
	}
	glDrawBuffers(_rtt.size(), attachments);
    }
}
void FrameBuffer::addBuffer(std::string&& name_, glm::vec2 resolution_) {
    if (_rtt.size() > 15) {
	std::cout << "opengl Does not support more than 15 framebuffer\n" << std::endl;
	return;
    }
    _rtt.emplace_back(_rtt.size(), std::move(name_), resolution_);
}
void FrameBuffer::addDepthBuffer(std::string&& name_, glm::vec2 resolution_) {
    _rttDepth.init(_rtt.size(), std::move(name_), resolution_);
}
void FrameBuffer::bindGBuffer() {
    unsigned int i = 0;
    for (RenderTexture<GL_RGB>& rtt: _rtt) {
	rtt.bind(i);
	++i;
    }
    _rttDepth.bind(i);
}
void FrameBuffer::disable() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
FrameBuffer::~FrameBuffer() {
    glDeleteFramebuffers(1, &_fbo);
}
