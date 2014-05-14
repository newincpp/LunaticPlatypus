#ifndef MESH_H_
# define MESH_H_
#include <initializer_list>
#include <vector>
#include "GL.h"
#include "IRender.hh"
#include "Shader.hh"

namespace GLnewin {
    template <unsigned int RENDERMODE = GL_TRIANGLES>
	class Mesh : public IRenderable {
	    private:
		enum {
		    TYPE = RENDERMODE
		};
		GLint _size;
		short _padding;
		GLuint vertexbuffer;
		Uniform<glm::mat4>* _model;
		inline GLuint _GPUAlloc(GLfloat* data, unsigned int size) noexcept {
		    GLuint id;
		    size *= sizeof(GLfloat);
		    glGenBuffers(1, &id); // malloc
		    glBindBuffer(GL_ARRAY_BUFFER, id); // say we are about to use an GL_ARRAY_BUFFER and this array is vertexbuffer
		    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // memset vertexbuffer
		    return id;
		}
		inline void _autoconfVBO() noexcept {
		    glEnableVertexAttribArray(0);
		    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		    glVertexAttribPointer(
			    0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
			    _padding, // size
			    GL_FLOAT, // type
			    GL_TRUE,// normalized?
			    0,// stride
			    (void*)0 // array buffer offset
			    );
		}
	    public:
		Mesh(std::initializer_list<GLfloat> l);
		Mesh(std::vector<GLfloat> l);
		Mesh(GLfloat* data, unsigned int size);
		virtual void draw() const noexcept;
		inline void setPadding(short n) { _padding = n; }
		void bindMatrixModifier(const Uniform<glm::mat4>&);
		void bindMatrixModifier(const glm::mat4& n) = delete;
		void refreshModifier(const glm::mat4& n);
	};
}

#include "Mesh.inl"

#endif /* !MESH_H_ */
