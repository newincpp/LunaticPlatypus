#ifndef CAMERA_H_
# define CAMERA_H_
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.hh"

namespace GLnewin {
    class ICamera {
    };
    class Camera {
	private:
	    Uniform<glm::mat4> _projection;
	    Uniform<glm::mat4> _view;
	    glm::vec3 _position;
	    glm::vec3 _lookAt;
	public:
	    explicit Camera(Shader&);
	    void lookAt(const glm::vec3&)noexcept;
	    void position(const glm::vec3&)noexcept;
	    void position(glm::vec3&& v)noexcept;
	    void setActive();
	    inline void reGenUniform(const Shader& s)noexcept{
		_projection = s.genUniform(glm::mat4(), "projection");
		_view = s.genUniform(glm::mat4(), "view");
		_projection = glm::perspectiveFov(45.0f, 1920.0f,1024.0f, 0.1f, 10.0f);
		_view = glm::lookAt(_position, _lookAt, glm::vec3(0.0, 1.0, 0.0));
	    }
    };
}

#endif /* !CAMERA_H_ */
