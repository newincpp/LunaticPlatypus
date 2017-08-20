#ifdef IMGUIENABLED
#include "imgui/imgui.h"
#endif
#include <iostream>
#include "Camera.hh"

// cinemascopeV1: 47/20 (2.35/1)
// cinemascopeV2: 239/100
// cinemascopeV3: 12/5 (2.40/1)
// wide: 16/9
// extendedWide: 16/10
// VGAStandard: 4/3
// portrait transformation-> 1/
//
// FOV = 2 * arctan(sensorSize / focalLengh) // focal length to fov for a given sensor size (8mm seems to be the standard)

Camera::Camera(FrameBuffer& fb_) :
    uView(glm::mat4(1.0f), "uView"),
    uProjection(glm::mat4(1.0f), "uProjection"),
    _position(15.0f, 5.0f, 15.0f),
    _forward(0.0f, 0.0f, 1.0f),
    _right(1.0f, 0.0f, 0.0f),
    _up(0.0f, 1.0f, 0.0f),
    _fov(1.745f),
    _clipPlane(0.1f, 100.0f), _gBuffer(fb_) {
    }

Camera::Camera(const Camera& c_) :
    uView(c_.uView),
    uProjection(c_.uProjection),
    _position(c_._position),
    _forward(c_._forward),
    _right(c_._right),
    _up(c_._up),
    _fov(c_._fov),
    _clipPlane(c_._clipPlane), _gBuffer(c_._gBuffer) {
    }

Camera::Camera(Camera&& c_) :
    uView(c_.uView),
    uProjection(c_.uProjection),
    _position(c_._position),
    _forward(c_._forward),
    _right(c_._right),
    _up(c_._up),
    _fov(c_._fov),
    _clipPlane(c_._clipPlane), _gBuffer(c_._gBuffer) {
    }

void Camera::setViewMatrix(glm::mat4&& m_) {
    uView._value.m4 = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    uView._value.m4 *= m_;
}

void Camera::lookAt(glm::vec3&& target_) {
    _forward = glm::normalize(target_ - _position);
    _right = glm::normalize(glm::cross(_forward, _up));
    _up = glm::cross(_right, _forward);
}
void Camera::setPos(glm::vec3&& newPos_) {
    _position = newPos_;
}
#include <glm/gtx/string_cast.hpp>
void Camera::use() {
    _gBuffer.enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _forward, _up);
    //autoRelocate(uCamera);
#ifdef IMGUIENABLED
    //ImGui::Begin("Camera Controls");
    ImGui::InputFloat3("Position", glm::value_ptr(_position));
    ImGui::InputFloat3("Target", glm::value_ptr(_forward));
    ImGui::InputFloat3("up", glm::value_ptr(_up));
    ImGui::InputFloat2("clipPlane", glm::value_ptr(_clipPlane));
    ImGui::InputFloat("fov", &_fov);
#endif
    //std::cout << "Pos: " << glm::to_string(_position) << '\n';
    //std::cout << "Right: " << glm::to_string(uView._value.m4[0]) << '\n';
    //std::cout << "Up: " <<  glm::to_string(uView._value.m4[1]) << '\n';
    //std::cout << "Forward: " <<  glm::to_string(uView._value.m4[2]) << '\n';
}
void Camera::updateUniform(unsigned int currentFrame) {
    uView._value.m4[0][0] = _right.x;
    uView._value.m4[1][0] = _right.y;
    uView._value.m4[2][0] = _right.z;
    uView._value.m4[0][1] = _up.x;
    uView._value.m4[1][1] = _up.y;
    uView._value.m4[2][1] = _up.z;
    uView._value.m4[0][2] = -_forward.x;
    uView._value.m4[1][2] = -_forward.y;
    uView._value.m4[2][2] = -_forward.z;
    uView._value.m4[3][0] = -glm::dot(_right, _position);
    uView._value.m4[3][1] = -glm::dot(_up, _position);
    uView._value.m4[3][2] =  glm::dot(_forward, _position);

    //uView._value.m4 = glm::lookAt(_position, _forward + _position, _up);
    uView.forceUpdateValue(currentFrame);
    uProjection.forceUpdateValue(currentFrame);
    //uProjection.updateValue(glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y), currentFrame);
}
void Camera::unUse() {
    _gBuffer.disable();
}
void Camera::bindFramebuffer() {
    _gBuffer.bindGBuffer();
}
void Camera::fieldOfview(float fov_) {
    _fov = fov_;
    uProjection.forceSetValue(glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y));
    std::cout << "fov: " << _fov << '\n';
}
void Camera::clipPlane(glm::vec2&& clip_) {
    _clipPlane = clip_;
    uProjection.forceSetValue(glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y));
    std::cout << "clip: " << _clipPlane[0] << ' ' << _clipPlane[1] << '\n';
}

void Camera::setup(glm::vec3&& fw_, glm::vec3&& right_, glm::vec3&& up_) {
    _forward = glm::normalize(fw_);
    _right = glm::normalize(glm::cross(_forward, up_));
    _up = glm::cross(_right, _forward);
}

void Camera::moveForward(float scal) {
    _position += _forward * scal;
}

void Camera::moveRight(float scal) {
    _position += _right * scal;
}

void Camera::moveUp(float scal) {
    _position += _up * scal;
}

void Camera::rotate(glm::vec3 a) {
    glm::mat4 rot = glm::rotate(glm::mat4(), a.x, glm::vec3(1.0, 0.0, 0.0));
    rot = glm::rotate(rot, a.y, glm::vec3(0.0, 1.0, 0.0));
    rot = glm::rotate(rot, a.z, glm::vec3(0.0, 0.0, 1.0));

    _right = (rot * glm::vec4(_right, 1.0f)).xyz();
    _up = (rot * glm::vec4(_up, 1.0f)).xyz();
    _forward = (rot * glm::vec4(_forward, 1.0f)).xyz();
}
