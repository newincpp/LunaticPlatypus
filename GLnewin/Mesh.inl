template <unsigned int RENDERMODE>
GLnewin::Mesh<RENDERMODE>::Mesh(std::initializer_list<GLfloat> l) : _size(l.size() / 3), _padding(3), _model(NULL) {
    GLfloat g_vertex_buffer_data[_size * 3];
    unsigned int i = 0;
    for (GLfloat x : l) {
	g_vertex_buffer_data[i] = x;
	++i;
    }
    vertexbuffer = _GPUAlloc(g_vertex_buffer_data, _size * 3);
    _autoconfVBO();
}

template <unsigned int RENDERMODE>
GLnewin::Mesh<RENDERMODE>::Mesh(std::vector<GLfloat> data) : _size(data.size() / 3), _padding(3), vertexbuffer(_GPUAlloc(&data[0], data.size())), _model(NULL) {
    _autoconfVBO();
}

template <unsigned int RENDERMODE>
GLnewin::Mesh<RENDERMODE>::Mesh(GLfloat* data, unsigned int size) : _size(size / 3), _padding(3), vertexbuffer(_GPUAlloc(data, size)), _model(NULL) {
    _autoconfVBO();
}

template <unsigned int RENDERMODE>
void GLnewin::Mesh<RENDERMODE>::bindMatrixModifier(const Uniform<glm::mat4>& n) {
    if (_model) {
	delete _model;
    }
    _model = new Uniform<glm::mat4>(n);
}

template <unsigned int RENDERMODE>
void GLnewin::Mesh<RENDERMODE>::refreshModifier(const glm::mat4& n) {
    (*_model) = n;
}

template <unsigned int RENDERMODE>
void GLnewin::Mesh<RENDERMODE>::draw() const noexcept {
    if (_model) {
	_model->upload();
    }
    glDrawArrays(TYPE, 0, _size); // Starting from vertex 0; 3 vertices total -> 1 triangle
}
