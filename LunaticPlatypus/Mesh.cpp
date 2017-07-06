#include "PlatypusHeart.hh"
#include "Mesh.hh"

Mesh::Mesh() : uMeshTransform(glm::mat4(), "uMeshTransform") {
    uMeshTransform._location = 1; // FIXME temporarily fixing auto location bug
}
Mesh::~Mesh() {
    Heart::getScene()->remove(&(uMeshTransform._value.m4));
}

#define BUFFER_STORAGE
#define STORAGE_FLAGS GL_MAP_READ_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT

void Mesh::uploadToGPU(std::vector<GLfloat>& vbo_, std::vector<GLuint>& ebo_) {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
#ifdef BUFFER_STORAGE
    glBufferStorage(GL_ARRAY_BUFFER, vbo_.size() * sizeof(__remove_reference__<decltype(vbo_)>::type::value_type), &(vbo_[0]), STORAGE_FLAGS);
#else
    glBufferData(GL_ARRAY_BUFFER, vbo_.size() * sizeof(__remove_reference__<decltype(vbo_)>::type::value_type), &(vbo_[0]), GL_STATIC_DRAW);
#endif

    //vertex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    //Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    //BiTangent
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
#ifdef BUFFER_STORAGE
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, ebo_.size() * sizeof(__remove_reference__<decltype(ebo_)>::type::value_type), &(ebo_[0]), STORAGE_FLAGS);
#else
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_.size() * sizeof(__remove_reference__<decltype(ebo_)>::type::value_type), &(ebo_[0]), GL_STATIC_DRAW);
#endif

    _size = ebo_.size();

    //std::cout << "vertex[" << vbo_.size() << "] = {\n";
    //for (GLfloat v : vbo_) {
    //    std::cout << v << ", ";
    //}
    //std::cout << "};\nindex[" << ebo_.size() << "] = {\n";
    //for (GLint v : ebo_) {
    //    std::cout << v << ", ";
    //}
    //std::cout << "};\n";
}

void Mesh::uploadVertexOnly(std::vector<GLfloat>& vbo_) {
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
#ifdef BUFFER_STORAGE
    glBufferStorage(GL_ARRAY_BUFFER, vbo_.size() * sizeof(__remove_reference__<decltype(vbo_)>::type::value_type), &(vbo_[0]), STORAGE_FLAGS);
#else
    glBufferData(GL_ARRAY_BUFFER, vbo_.size() * sizeof(__remove_reference__<decltype(vbo_)>::type::value_type), &(vbo_[0]), GL_STATIC_DRAW);
#endif

    //vertex
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    //Texture coordinates
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
    //BiTangent
    //glEnableVertexAttribArray(3);
    //glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));
}

void Mesh::uploadElementOnly(std::vector<GLuint>& ebo_, GLuint vbo_, GLuint vao_) {
    _vbo = vbo_;
    _vao = vao_;

    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
#ifdef BUFFER_STORAGE
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, ebo_.size() * sizeof(__remove_reference__<decltype(ebo_)>::type::value_type), &(ebo_[0]), STORAGE_FLAGS);
#else
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, ebo_.size() * sizeof(__remove_reference__<decltype(ebo_)>::type::value_type), &(ebo_[0]), GL_STATIC_DRAW);
#endif
    _size = ebo_.size();
}

void Mesh::GpuFree() {
    freeVao();
    freeVbo();
    freeEbo();
}

void Mesh::freeVao() {
    glDeleteVertexArrays(1, &_vao);
}

void Mesh::freeVbo() {
    glDeleteBuffers(1, &_vbo);
}

void Mesh::freeEbo() {
    glDeleteBuffers(1, &_ebo);
}

void Mesh::render() {
    uMeshTransform.upload();
    glBindVertexArray(_vao);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);

    glDrawElements(GL_TRIANGLES, _size, GL_UNSIGNED_INT, 0); // TODO implement multidraw architecture
}

#include <glm/gtx/string_cast.hpp>

void Mesh::linkNode(Node& n_) {
    n_.linkWorldTransform(&(uMeshTransform._value.m4));
}
