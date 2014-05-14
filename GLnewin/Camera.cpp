#include "Camera.hh"

GLnewin::Camera::Camera(Shader& s) : _projection(s.genUniform(glm::mat4(), "projection")), _view(s.genUniform(glm::mat4(), "view")), _position(0,0,0), _res(1920, 1080) {
    _projection = glm::perspectiveFov(60.0f, _res.x,_res.y, 0.1f, 1000.0f);
    lookAt(glm::vec3(0,0,1));
}

void GLnewin::Camera::lookAt(const glm::vec3& v) noexcept {
    _lookAt = v;
    _view = glm::lookAt(_position, _lookAt, glm::vec3(0.0, 1.0, 0.0));
}

void GLnewin::Camera::projection(float fov, float min, float max) noexcept {
    _projection = glm::perspectiveFov(fov, _res.x, _res.y, min, max);
}

void GLnewin::Camera::position(const glm::vec3& v) noexcept {
    _position = v;
    _view = glm::lookAt(_position, _lookAt, glm::vec3(0.0, 1.0, 0.0));
}

void GLnewin::Camera::position(glm::vec3&& v) noexcept {
    _position = v;
    _view = glm::lookAt(_position, _lookAt, glm::vec3(0.0, 1.0, 0.0));
}

void GLnewin::Camera::setActive() {
    _view.upload();
    _projection.upload();
}
