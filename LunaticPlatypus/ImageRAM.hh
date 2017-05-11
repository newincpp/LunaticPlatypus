#pragma once
#include "glew.h"

class ImageRAM {
    private:
	const GLenum _type;
	GLuint _imageId;
    public:
	ImageRAM(GLuint = GL_RGBA16F);
	void useRW();
	void useR();
	void useW();
};
