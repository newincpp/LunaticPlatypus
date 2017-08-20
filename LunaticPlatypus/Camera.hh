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
	glm::vec3 _position;
	glm::vec3 _forward;
	glm::vec3 _right;
	glm::vec3 _up;
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
        void setup(glm::vec3&&,glm::vec3&&,glm::vec3&&);
	//void up(glm::vec3&&);
	//void right(glm::vec3&&);
	//void forward(glm::vec3&&);
	void fieldOfview(float);
	void clipPlane(glm::vec2&&);
	void setViewMatrix(glm::mat4&&);
	void use();
	void updateUniform(unsigned int currentFrame = 0);
	void unUse();
	void bindFramebuffer();

        void moveForward(float);
        void moveRight(float);
        void moveUp(float);
        void rotate(glm::vec3);
};
