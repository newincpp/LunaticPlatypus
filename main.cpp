#include <initializer_list>
#include <iostream>
#include "GL.h"
#include "RenderSystem.hh"

class mesh : public GLnewin::IRenderable {
    private:
	GLint _size;
	GLuint vertexbuffer;
	inline GLuint GPUAlloc(GLfloat* data, unsigned int size) noexcept {
	    GLuint id;
	    size *= sizeof(GLfloat);
	    glGenBuffers(1, &id); // malloc
	    glBindBuffer(GL_ARRAY_BUFFER, id); // say we are about to use an GL_ARRAY_BUFFER and this array is vertexbuffer
	    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); // memset vertexbuffer
	    return id;
	}
    public:
	mesh(std::initializer_list<GLfloat> l) : _size(l.size() / 3) {
	    GLfloat g_vertex_buffer_data[_size * 3];
	    unsigned int i = 0;
	    for (GLfloat x : l) {
		g_vertex_buffer_data[i] = x;
		++i;
	    }
	    vertexbuffer = GPUAlloc(g_vertex_buffer_data, _size * 3);
	}
	mesh(GLfloat* data, unsigned int size) : _size(size / 3), vertexbuffer(GPUAlloc(data, size)) {
	}
	virtual void draw() noexcept {
	    glEnableVertexAttribArray(0);
	    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	    glVertexAttribPointer(
		    0, // attribute 0. No particular reason for 0, but must match the layout in the shader.
		    _size, // size (number of vertex ?)
		    GL_FLOAT, // type
		    GL_TRUE,// normalized?
		    0,// stride
		    (void*)0 // array buffer offset
		    );
	    glDrawArrays(GL_TRIANGLES, 0, _size); // Starting from vertex 0; 3 vertices total -> 1 triangle
	}
};

int main() {
    GLnewin::IRendertarget* r = new GLnewin::Window(100, 100, false, "demo");
    r->pushRenderCandidate(new mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}));
    while (true) {
	r->render();
    }
    delete r;
}
