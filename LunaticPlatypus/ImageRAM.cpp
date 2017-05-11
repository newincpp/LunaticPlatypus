#include "ImageRAM.hh"

ImageRAM::ImageRAM(GLuint type_) : _type(type_) {
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &_imageId);
    glBindTexture(GL_TEXTURE_2D, _imageId);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

    //glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA16UI, 512, 512); // int
    glTexStorage2D( GL_TEXTURE_2D, 1, _type, 512, 512); // float
    glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageRAM::useRW() {
    glBindImageTexture(1, _imageId, 0, GL_FALSE, 0, GL_READ_WRITE, _type);
}
void ImageRAM::useR() {
    glBindImageTexture(1, _imageId, 0, GL_FALSE, 0, GL_READ_ONLY, _type);
}
void ImageRAM::useW() {
    glBindImageTexture(1, _imageId, 0, GL_FALSE, 0, GL_WRITE_ONLY, _type);
}
