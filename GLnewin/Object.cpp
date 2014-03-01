#include "glm/gtc/matrix_transform.hpp"
#include "Object.hh"

GLnewin::Object::Object() : _mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}), _rotPosScale(NULL) {
    _shader.setVertex(GLnewin::Shader::fileToString("vert.glsl"));
    _shader.setFragment(GLnewin::Shader::fileToString("frag.glsl"));
    _shader.link();
    _mesh.setShader(&_shader);
    _rotPosScale = new Uniform<glm::mat4>(_shader.genUniform(glm::mat4(), "object"));
}

void GLnewin::Object::draw() noexcept {
    _mesh.draw();
}

void GLnewin::Object::setPos(const glm::vec3& v) noexcept {
    _pos = v;
    *_rotPosScale = glm::translate(glm::mat4(), _pos);
    _rotPosScale->upload();
}

void GLnewin::Object::setRot(const glm::vec3& v) noexcept {
    _rot = v;
}

void GLnewin::Object::setScale(const glm::vec3& v) noexcept {
    _scale = v;
}
