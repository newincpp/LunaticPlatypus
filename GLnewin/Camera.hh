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
	    glm::vec3 _position;
	    glm::vec3 _lookAt;
	public:
	    explicit Camera(Shader&);
	    void lookAt(const glm::vec3&)noexcept;
	    void position(const glm::vec3&)noexcept;
	    void setActive();
    };
}

#endif /* !CAMERA_H_ */
