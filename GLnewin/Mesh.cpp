#include "Mesh.hh"

GLnewin::Mesh::Mesh(std::initializer_list<GLfloat> l) : _size(l.size() / 3), _padding(3), _model(NULL) {
    GLfloat g_vertex_buffer_data[_size * 3];
    unsigned int i = 0;
    for (GLfloat x : l) {
	g_vertex_buffer_data[i] = x;
	++i;
    }
    vertexbuffer = _GPUAlloc(g_vertex_buffer_data, _size * 3);
}

GLnewin::Mesh::Mesh(std::vector<GLfloat> data) : _size(data.size() / 3), _padding(3), vertexbuffer(_GPUAlloc(&data[0], data.size())), _model(NULL) {
}

GLnewin::Mesh::Mesh(GLfloat* data, unsigned int size) : _size(size / 3), _padding(3), vertexbuffer(_GPUAlloc(data, size)), _model(NULL) {
}

void GLnewin::Mesh::bindMatrixModifier(const Uniform<glm::mat4>& n) {
    if (_model) {
	delete _model;
    }
    _model = new Uniform<glm::mat4>(n);
}

void GLnewin::Mesh::refreshModifier(const glm::mat4& n) {
    (*_model) = n;
}

void GLnewin::Mesh::draw() const noexcept {
    if (_model) {
	_model->upload();
    }
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
    glDrawArrays(GL_TRIANGLES, 0, _size); // Starting from vertex 0; 3 vertices total -> 1 triangle
}
