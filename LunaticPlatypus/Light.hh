#include <glm/glm.hpp>
#include "ImageRAM.hh"
#include "Uniform.hh"

class Light {
    public:
	static ImageRAM* _illuminationBuffer;
	Uniform uView;
	Uniform uProjection;

	glm::vec3 _target;
	glm::vec3 _position;
	glm::vec3 _upVector;
	glm::vec2 _clipPlane;
	float _fov;
	bool _castShadows;
	explicit Light();
	void use();
};
