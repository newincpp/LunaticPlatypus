#pragma once
#include "Uniform.hh"
#include "FrameBuffer.hh"
#include "shader.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
    public:
	Uniform uView;
	Uniform uProjection;
    private:
	glm::vec3 _target;
	glm::vec3 _position;
	glm::vec3 _upVector;
	float _fov;
	glm::vec2 _clipPlane;
	FrameBuffer& _gBuffer;
    public:
	Camera(FrameBuffer&);
	Camera(const Camera&);
	Camera(Camera&&);
	void operator=(const Camera&&) = delete;
	void lookAt(glm::vec3&&);
	void setPos(glm::vec3&&);
	void fieldOfview(float);
	void clipPlane(glm::vec2&&);
	void upVector(glm::vec3&&);
	void setMatrix(glm::mat4&&);
	void use();
	void updateUniform(unsigned int currentFrame = 0);
	void unUse();
	void bindFramebuffer();
};
