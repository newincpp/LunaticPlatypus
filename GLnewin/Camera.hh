#ifndef CAMERA_H_
# define CAMERA_H_
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Shader.hh"

namespace GLnewin {
    class Camera {
	private:
	    Uniform<glm::mat4> _projection;
	    Uniform<glm::mat4> _view;
	    glm::vec3 _position;
	    glm::vec3 _lookAt;
	    glm::vec2 _res;
	public:
	    explicit Camera(Shader&);
	    void lookAt(const glm::vec3&)noexcept;
	    void projection(float fov, float = 0.1, float = 1000)noexcept;
	    void position(const glm::vec3&)noexcept;
	    void position(glm::vec3&& v)noexcept;
	    void setActive();
	    template <typename __DISP_TYPE> void syncronizeRez(__DISP_TYPE&&);
	    template <typename __DISP_TYPE> void syncronizeRez(const __DISP_TYPE&);
	    inline void reGenUniform(const Shader& s)noexcept{
		_projection = s.genUniform(glm::mat4(), "projection");
		_view = s.genUniform(glm::mat4(), "view");
		_projection = glm::perspectiveFov(45.0f, 1920.0f,1024.0f, 0.1f, 10.0f);
		_view = glm::lookAt(_position, _lookAt, glm::vec3(0.0, 1.0, 0.0));
	    }
    };
}

#include "Camera.inl"

#endif /* !CAMERA_H_ */
