#include <imgui.h>
#include <iostream>
#include "Camera.hh"

Camera::Camera() : 
    uCamera(1, glm::mat4(1.0f)), 
    _target(0.0f, 0.0f, 0.0f),
    _position(15.0f, 5.2f, 15.0f),
    _upVector(0.0f, 1.0f, 0.0f),
    _fov(1.745f),
    _clipPlane(0.1f, 1000.0f), _gBuffer() {
	_gBuffer.addBuffer("gPosition");
	_gBuffer.addBuffer("gNormal");
	_gBuffer.addBuffer("gAlbedoSpec");
	_gBuffer.addDepthBuffer("gDepth");
	_gBuffer.enable();
    }

Camera::Camera(const Camera& c_) :
    uCamera(c_.uCamera), 
    _target(c_._target),
    _position(c_._position),
    _upVector(c_._upVector),
    _fov(c_._fov),
    _clipPlane(c_._clipPlane), _gBuffer() {
	//_gBuffer.addBuffer("gPosition", glm::vec2(64,64));
	//_gBuffer.addBuffer("gNormal", glm::vec2(64,64));
	_gBuffer.addBuffer("gAlbedoSpec", glm::vec2(64,64));
	_gBuffer.addDepthBuffer("gDepth", glm::vec2(64,64));
	_gBuffer.enable();
    }

Camera::Camera(Camera&& c_) :
    uCamera(c_.uCamera), 
    _target(c_._target),
    _position(c_._position),
    _upVector(c_._upVector),
    _fov(c_._fov),
    _clipPlane(c_._clipPlane), _gBuffer() {
	//_gBuffer.addBuffer("gPosition", glm::vec2(64,64));
	//_gBuffer.addBuffer("gNormal", glm::vec2(64,64));
	_gBuffer.addBuffer("gAlbedoSpec", glm::vec2(32,32));
	_gBuffer.addDepthBuffer("gDepth", glm::vec2(64,64));
	_gBuffer.enable();
    }

void Camera::setMatrix(glm::mat4&& m_) {
    uCamera = m_;
    glm::vec4 p = m_ * glm::vec4(0.0f,0.0f,0.0f,1.0f) ;
    std::cout << "setMatrix pos: " << p[0] << ' ' << p[1] << ' ' << p[2] << '\n';
}

void Camera::lookAt(glm::vec3&& target_) {
    _target = target_;
    std::cout << "targ: " << _target[0] << ' ' << _target[1] << ' ' << _target[2] << '\n';
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::setPos(glm::vec3&& newPos_) {
    _position = newPos_;
    std::cout << "pos: " << _position[0] << ' ' << _position[1] << ' ' << _position[2] << '\n';
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
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
    uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt(_position, _target, _upVector);
    //ImGui::End();
    uCamera.upload();
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
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::clipPlane(glm::vec2&& clip_) {
    _clipPlane = clip_;
    std::cout << "clip: " << _clipPlane[0] << ' ' << _clipPlane[1] << '\n';
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
void Camera::upVector(glm::vec3&& up_) {
    _upVector = glm::normalize(up_);
    std::cout << "up: " << _upVector[0] << ' ' << _upVector[1] << ' ' << _upVector[2] << '\n';
    //uCamera = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y) * glm::lookAt( _position, _target, _upVector);
}
