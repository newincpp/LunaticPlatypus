#include <iostream>
#include "Texture.hh"

template <>
void GLnewin::Texture<1>::_bind() {
    std::cout << "1D" << std::endl;
    glBindTexture(GL_TEXTURE_1D, _id);
}

template <>
void GLnewin::Texture<2>::_bind() {
    std::cout << "2D" << std::endl;
    glBindTexture(GL_TEXTURE_2D, _id);
}

template <>
void GLnewin::Texture<3>::_bind() {
    std::cout << "3D" << std::endl;
    glBindTexture(GL_TEXTURE_3D, _id);
}
