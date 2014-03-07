#ifndef OBJECT_H_
# define OBJECT_H_
#include "glm/glm.hpp"
#include "IRender.hh"
#include "Camera.hh"
#include "Mesh.hh"

namespace GLnewin {
    class Object : public IRenderable {
	private:
	    GLnewin::Mesh _mesh;
	    GLnewin::Shader _shader;
	    glm::vec3 _pos;
	    glm::vec3 _rot;
	    glm::vec3 _scale;
	    Uniform<glm::mat4>* _rotPosScale;
	public:
	    explicit Object();
	    virtual void draw()noexcept;
	    inline Camera genCamera() noexcept { return Camera(_shader); }
	    void setPos(const glm::vec3&) noexcept;
	    void setRot(const glm::vec3&) noexcept;
	    void setScale(const glm::vec3&) noexcept;
	    bool DoTheImportThing(const std::string& pFile);
    };
}

#endif /* !OBJECT_H_ */
