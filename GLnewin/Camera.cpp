#include "Camera.hh"

GLnewin::Camera::Camera(Shader& s) : _projection(s.genUniform(glm::mat4(), "projection")), _modelview(s.genUniform(glm::mat4(), "modelview")) {
    //_projection = glm::perspective(45.0f, 1920.0f/1024.0f, 0.1f, 10.0f);
    _projection = glm::perspectiveFov(45.0f, 1920.0f,1024.0f, 0.1f, 10.0f);
    _modelview = glm::mat4();
    _projection.upload();
    _modelview.upload();
}

void GLnewin::Camera::lookAt(const glm::vec3& v) noexcept {
    _lookAt = v;
    _modelview = glm::lookAt(_lookAt, _position, glm::vec3(0.0, 1.0, 0.0));
}

void GLnewin::Camera::position(const glm::vec3& v) noexcept {
    _position = v;
    _modelview = glm::lookAt(_lookAt, _position, glm::vec3(0.0, 1.0, 0.0));
}

void GLnewin::Camera::setActive() {
    _modelview.upload();
    _projection.upload();
}
