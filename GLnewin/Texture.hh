#include <string>
#include "glew.h"
#include <glm/glm.hpp>

class Texture {
    public:
	GLuint _textureID;
	Texture();
	void load(const std::string&);
	void load(float*, unsigned int, unsigned int);
	void load();
};
