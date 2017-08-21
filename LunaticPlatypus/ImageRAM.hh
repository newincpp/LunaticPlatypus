#pragma once
#include "glew.h"
#include <glm/glm.hpp>

class ImageRAM {
    private:
	const GLenum _type;
	GLuint _imageId;
        void create(GLenum);
    public:
	ImageRAM(GLuint = GL_RGBA16F, glm::vec2 = glm::vec2(1920, 1080));
	ImageRAM(GLuint, glm::vec3);
	void useRW(GLuint unit);
	void useR(GLuint unit);
	void useW(GLuint unit);
	void sync();
};
