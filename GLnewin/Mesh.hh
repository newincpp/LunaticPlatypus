#ifndef MESH_H_
# define MESH_H_
#include <initializer_list>
#include <vector>
#include "GL.h"
#include "IRender.hh"
#include "Shader.hh"

namespace GLnewin {
    class Mesh : public IRenderable {
	private:
	    GLint _size;
	    short _padding;
	    GLuint _ebo;
	    GLuint vertexbuffer;
	    inline GLuint _GPUAlloc(GLfloat* data, unsigned int size) noexcept {
		GLuint id;
		size *= sizeof(GLfloat);
		glGenBuffers(1, &id); // malloc
		glBindBuffer(GL_ARRAY_BUFFER, id); // say we are about to use an GL_ARRAY_BUFFER and this array is vertexbuffer
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // memset vertexbuffer
		return id;
	    }
	public:
	    Mesh(std::initializer_list<GLfloat> l);
	    Mesh(std::vector<GLfloat> l);
	    Mesh(GLfloat* data, unsigned int size);
	    virtual void draw() const noexcept;
	    inline void setPadding(short n) { _padding = n; }
    };
}

#endif /* !MESH_H_ */
