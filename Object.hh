#ifndef OBJECT_H_
# define OBJECT_H_
#include "IRender.hh"
#include "Camera.hh"
#include "Mesh.hh"

namespace GLnewin {
    class Object : public IRenderable {
	private:
	    GLnewin::Mesh _mesh;
	    GLnewin::Shader _shader;
	public:
	    explicit Object();
	    virtual void draw()noexcept;
	    inline Camera genCamera() noexcept { return Camera(_shader); }
    };
}

#endif /* !OBJECT_H_ */
