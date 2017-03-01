#pragma once
#include <iostream>
#include <string>
#include <utility>
#include "glew.h"
#include <glm/glm.hpp>

enum {
    DEPTH,
    COLOR
};

template <GLuint MODE>
class RenderTexture {
    private:
    	GLenum _genttachmentEnum(unsigned short);
	void _TexImage2D(glm::vec2& resolution_);
    public:
	GLuint _id;
	GLuint _attachment;
	std::string _name;
	RenderTexture();
	~RenderTexture();
	RenderTexture(unsigned short attachment_, std::string&&, glm::vec2 resolution_ = glm::vec2(1920, 1080));
	void init(unsigned short attachment_, std::string&&, glm::vec2 resolution_ = glm::vec2(1920, 1080));
	void bind(unsigned int i_);
	inline GLuint getAttachment() {
	    return _attachment;
	}
	inline GLuint getId() {
	    return _id;
	}
};

template <GLuint MODE>
RenderTexture<MODE>::RenderTexture() : _id(0), _name("") {
}

template <GLuint MODE>
RenderTexture<MODE>::~RenderTexture() {
    if (_id) {
	glDeleteTextures(1, &_id);
    } else {
    	std::cout << "trying to delete not init texture" << std::endl;
    }
}

template <GLuint MODE>
RenderTexture<MODE>::RenderTexture(unsigned short attachment_, std::string&& name_, glm::vec2 resolution_) : _name(name_) {
    init(attachment_, std::move(name_), resolution_);
}

template <GLuint MODE>
void RenderTexture<MODE>::init(unsigned short attachment_, std::string&& name_, glm::vec2 resolution_) {
    _name = std::move(name_);
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    _TexImage2D(resolution_);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    _attachment = _genttachmentEnum(attachment_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, _attachment, GL_TEXTURE_2D, _id, 0);
}

template <GLuint MODE>
void RenderTexture<MODE>::bind(unsigned int i_) {
    glActiveTexture(GL_TEXTURE0 + i_);
    glBindTexture(GL_TEXTURE_2D, _id);
    glGenerateMipmap(GL_TEXTURE_2D); // TODO fast ugly this is 1 frame old

    GLint programId;
    glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
    glUniform1i(glGetUniformLocation(programId, _name.c_str()), i_);
}
