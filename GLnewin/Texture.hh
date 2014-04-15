#ifndef TEXTURE_H_
# define TEXTURE_H_

#include "GL.h"

namespace GLnewin {
    template <unsigned short DIMENTION = 2>
	class Texture {
	    private:
		GLuint _id;
		void _bind();
	    public:
		Texture() {
		    static_assert(DIMENTION <= 3, "cannot create a texture with more than 4 dimention");
		    static_assert(DIMENTION != 0, "cannot create a texture with 0 dimention");
		    glGenTextures(1, &_id);
		    _bind();
		}
	};
    typedef Texture<1> Texture1D;
    typedef Texture<2> Texture2D;
    typedef Texture<3> Texture3D;
};


#endif /* !TEXTURE_H_ */
