#include "glm/gtc/matrix_transform.hpp"
#include "Camera.hh"

GLnewin::Camera::Camera(Shader& s) : _linkedShader(s), _projection(s.genUniform(glm::mat4(), "projection")), _modelview(s.genUniform(glm::mat4(), "modelview")) {
    _projection = glm::perspective(35.0f, 1.0f, 0.1f, 100.0f);
    _modelview = glm::translate(glm::mat4(), glm::vec3(0.0f,0,1));
    //_projection.upload();
    _modelview.upload();
}

void GLnewin::Camera::lookAt(glm::vec3) noexcept {
}

void GLnewin::Camera::position(glm::vec3) noexcept {
}
