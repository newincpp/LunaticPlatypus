#pragma once
#include "glew.h"
#include <glm/glm.hpp>

class ImageRAM {
    private:
	const GLenum _type;
	GLuint _imageId;
    public:
	ImageRAM(GLuint = GL_RGBA16F, glm::vec2 = glm::vec2(1920, 1080));
	void useRW();
	void useR();
	void useW();
	void sync();
};
