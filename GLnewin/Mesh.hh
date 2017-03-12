#pragma once 

#include <vector>
#include "glew.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Uniform.hh"

template<typename InputType_>
struct __remove_reference__ {
    typedef InputType_ type;
};
template<typename InputType_>
struct __remove_reference__<InputType_&> {
    typedef InputType_ type;
};
template<typename InputType_>
struct __remove_reference__<InputType_&&> {
    typedef InputType_ type;
};


class Mesh {
    public:
	GLuint _vao;
	GLuint _vbo;
	GLuint _ebo;
	int _size;
	Uniform uMeshTransform;
	//std::string _name;
	Mesh();
	void uploadToGPU(std::vector<GLfloat>&, std::vector<GLuint>&);
	void uploadElementOnly(std::vector<GLuint>&, GLuint, GLuint);
	void uploadVertexOnly(std::vector<GLfloat>&);
	void render();
	void freeVao();
	void freeVbo();
	void freeEbo();
	void GpuFree();
};
