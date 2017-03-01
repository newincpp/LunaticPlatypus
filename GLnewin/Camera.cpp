#include <imgui.h>
#include <iostream>
#include "Camera.hh"

Camera::Camera(FrameBuffer& fb_) :
    uView(glm::mat4(1.0f), "uView"),
    uProjection(glm::mat4(1.0f), "uProjection"),
    _target(0.0f, 0.0f, 0.0f),
    _position(15.0f, 5.2f, 15.0f),
    _upVector(0.0f, 1.0f, 0.0f),
    _fov(1.745f),
    _clipPlane(0.1f, 100.0f), _gBuffer(fb_) {
    }

Camera::Camera(const Camera& c_) :
    uView(c_.uView),
    uProjection(c_.uProjection),
    _target(c_._target),
    _position(c_._position),
    _upVector(c_._upVector),
    _fov(c_._fov),
    _clipPlane(c_._clipPlane), _gBuffer(c_._gBuffer) {
    }

Camera::Camera(Camera&& c_) :
    uView(c_.uView),
    uProjection(c_.uProjection),
    _target(c_._target),
    _position(c_._position),
    _upVector(c_._upVector),
    _fov(c_._fov),
    _clipPlane(c_._clipPlane), _gBuffer(c_._gBuffer) {
    }

void Camera::setMatrix(glm::mat4&& m_) {
    glm::vec4 p = m_ * glm::vec4(0.0f,0.0f,0.0f,1.0f) ;
    std::cout << "setMatrix pos: " << p[0] << ' ' << p[1] << ' ' << p[2] << '\n';
}

void Camera::lookAt(glm::vec3&& target_) {
    _target = target_;
}
void Camera::setPos(glm::vec3&& newPos_) {
    _position = newPos_;
}
void Camera::use() {
    _gBuffer.enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
    //autoRelocate(uCamera);
    //ImGui::Begin("Camera Controls");
    ImGui::InputFloat3("Position", glm::value_ptr(_position));
    ImGui::InputFloat3("Target", glm::value_ptr(_target));
    ImGui::InputFloat3("upVector", glm::value_ptr(_upVector));
    ImGui::InputFloat2("clipPlane", glm::value_ptr(_clipPlane));
    ImGui::InputFloat("fov", &_fov);
}
void Camera::updateUniform(unsigned int currentFrame) {
    uView.updateValue(glm::lookAt(_position, _target, _upVector), currentFrame);
    uProjection.updateValue(glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y), currentFrame);
}
void Camera::unUse() {
    _gBuffer.disable();
}
void Camera::bindFramebuffer() {
    _gBuffer.bindGBuffer();
}
void Camera::fieldOfview(float fov_) {
    _fov = fov_;
    std::cout << "fov: " << _fov << '\n';
}
void Camera::clipPlane(glm::vec2&& clip_) {
    _clipPlane = clip_;
    std::cout << "clip: " << _clipPlane[0] << ' ' << _clipPlane[1] << '\n';
}
void Camera::upVector(glm::vec3&& up_) {
    _upVector = glm::normalize(up_);
    std::cout << "up: " << _upVector[0] << ' ' << _upVector[1] << ' ' << _upVector[2] << '\n';
}
