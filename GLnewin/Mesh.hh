#ifndef MESH_H_
# define MESH_H_
#include <initializer_list>
#include <vector>
#include "GL.h"
#include "IRender.hh"
#include "Shader.hh"

namespace GLnewin {
    class Mesh : public GLnewin::IRenderable {
	private:
	    GLint _size;
	    GLuint vertexbuffer;
	    Shader* _shad;
	    inline GLuint _GPUAlloc(GLfloat* data, unsigned int size) noexcept {
		GLuint id;
		size *= sizeof(GLfloat);
		glGenBuffers(1, &id); // malloc
		glBindBuffer(GL_ARRAY_BUFFER, id); // say we are about to use an GL_ARRAY_BUFFER and this array is vertexbuffer
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // memset vertexbuffer
		return id;
	    }
	public:
	    explicit Mesh(std::initializer_list<GLfloat> l);
	    explicit Mesh(std::vector<GLfloat> l);
	    explicit Mesh(GLfloat* data, unsigned int size);
	    virtual void draw() noexcept;

	    void setShader(Shader*) noexcept;
	    inline Shader* getShader() noexcept { return _shad; }
    };
}

#endif /* !MESH_H_ */
