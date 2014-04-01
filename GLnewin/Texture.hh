#ifndef TEXTURE_H_
# define TEXTURE_H_

#include "GL.h"

namespace GLnewin {
    template <unsigned short DIMENTION = 2>
	class Texture {
	    static_assert(DIMENTION <= 3, "cannot create a texture with more than 4 dimention");
	    static_assert(DIMENTION != 0, "cannot create a texture with 0 dimention");
	    private:
		GLuint _id;
		void _bind();
	    public:
		Texture();
	};
};

template <>
void GLnewin::Texture<1>::_bind() {
    std::cout << "2D" << std::endl;
    glBindTexture(GL_TEXTURE_1D, _id);
}
template <>
void GLnewin::Texture<2>::_bind() {
    std::cout << "2D" << std::endl;
    glBindTexture(GL_TEXTURE_2D, _id);
}
template <>
void GLnewin::Texture<3>::_bind() {
    std::cout << "2D" << std::endl;
    glBindTexture(GL_TEXTURE_3D, _id);
}

template <unsigned short DIMENTION>
GLnewin::Texture<DIMENTION>::Texture() {
    glGenTextures(1, &_id);
    _bind();
}

#endif /* !TEXTURE_H_ */
