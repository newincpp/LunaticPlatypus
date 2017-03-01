#pragma once
#include <string>
#include "glew.h"
#include <glm/glm.hpp>

// rewrite this using opengl 4.5 style
// exemples here: https://bcmpinc.wordpress.com/2015/10/07/copy-a-texture-to-screen/#more-365
// and here: https://cpp-rendering.io/opengl-azdo-bindless-textures/

class Texture {
    public:
	GLuint _textureID;
	Texture();
	void load(const std::string&);
	void load(float*, unsigned int, unsigned int);
	void load();
};
