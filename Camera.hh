#ifndef CAMERA_H_
# define CAMERA_H_
#include "glm/glm.hpp"
#include "Shader.hh"

namespace GLnewin {
    class Camera {
	private:
	    Shader& _linkedShader;
	    Uniform<glm::mat4> _projection;
	    Uniform<glm::mat4> _modelview;
	public:
	    explicit Camera(Shader&);
	    void lookAt(glm::vec3)noexcept;
	    void position(glm::vec3)noexcept;
    };
}

#endif /* !CAMERA_H_ */
