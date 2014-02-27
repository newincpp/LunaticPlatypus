#include "Mesh.hh"

GLnewin::Mesh::Mesh(std::initializer_list<GLfloat> l) : _size(l.size() / 3), _shad(NULL) {
    GLfloat g_vertex_buffer_data[_size * 3];
    unsigned int i = 0;
    for (GLfloat x : l) {
	g_vertex_buffer_data[i] = x;
	++i;
    }
    vertexbuffer = GPUAlloc(g_vertex_buffer_data, _size * 3);
}

GLnewin::Mesh::Mesh(GLfloat* data, unsigned int size) : _size(size / 3), vertexbuffer(GPUAlloc(data, size)) {
}

void GLnewin::Mesh::draw() noexcept {
    if (_shad) {
	_shad->use();
    }
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

void GLnewin::Mesh::setShader(Shader* s) noexcept {
    _shad = s;
}
