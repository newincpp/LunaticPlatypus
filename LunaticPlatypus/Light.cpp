#include <iostream>
#ifdef IMGUIENABLED
#include <imgui.h>
#endif
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Light.hh"

ImageRAM* Light::_illuminationBuffer = nullptr;
Light::Light() : uView(glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 100.0f), "uView"),
    uProjection(glm::lookAt(glm::vec3(-2.0f, 4.0f, -1.0f), glm::vec3( 0.0f, 0.0f,  0.0f), glm::vec3( 0.0f, 1.0f,  0.0f)), "uProjection"),
    _castShadows(true) {
    uView._location = 2; // FIXME temporarily fixing auto location bug
    uProjection._location = 3; // FIXME temporarily fixing auto location bug
    if (!_illuminationBuffer) {
	_illuminationBuffer = new ImageRAM();
    }
    //uView = glm::perspective(90.0f, 1920.0f / 1080.0f, 0.1f, 100.0f);
    //uProjection = glm::lookAt(glm::vec3(1.5f, 36.0f, -25.0f), glm::vec3(1.0f, 13.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));

    _target = glm::vec3(1.0f, 13.0f, 0.0f);
    _position = glm::vec3(-10.f, 28.0f, -7.0f);
    _upVector = glm::vec3(0.0f, 1.0f, 0.0f);
    _clipPlane = glm::vec2(0.1f, 100.0f);
    _fov = 90.0f;
}

void Light::use() {
    //uView = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 900.0f);

#ifdef IMGUIENABLED
    ImGui::InputFloat3("Light Position", glm::value_ptr(_position));
    ImGui::InputFloat3("Light Target", glm::value_ptr(_target));
    ImGui::InputFloat3("Light upVector", glm::value_ptr(_upVector));
    ImGui::InputFloat2("Light clipPlane", glm::value_ptr(_clipPlane));
    ImGui::InputFloat("Light fov", &_fov);
#endif

    uView = glm::lookAt(_position, _target, _upVector);
    uProjection = glm::perspective(_fov, 1920.0f / 1080.0f, _clipPlane.x, _clipPlane.y);

    uView.upload();
    uProjection.upload();
    _illuminationBuffer->useW();
}
