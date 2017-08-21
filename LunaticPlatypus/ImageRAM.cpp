#include "ImageRAM.hh"

ImageRAM::ImageRAM(GLuint type_, glm::vec2 resolution_) : _type(type_) {
    glActiveTexture(GL_TEXTURE0);
    create(GL_TEXTURE_2D);
    glTexStorage2D(GL_TEXTURE_2D, 1, _type, resolution_.x, resolution_.y); // float
    glBindTexture(GL_TEXTURE_2D, 0);
}

ImageRAM::ImageRAM(GLuint type_, glm::vec3 resolution_) : _type(type_) {
    glActiveTexture(GL_TEXTURE0);
    create(GL_TEXTURE_3D);
    glTexStorage3D(GL_TEXTURE_3D, 1, _type, resolution_.x, resolution_.y, resolution_.z); // float
    glBindTexture(GL_TEXTURE_3D, 0);
}

void ImageRAM::create(GLenum type) {
    glGenTextures(1, &_imageId);
    glBindTexture(type, _imageId);

    glTexParameteri(type, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri(type, GL_TEXTURE_MAX_LEVEL, 0 );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}


void ImageRAM::useRW(GLuint unit) {
    glBindImageTexture(unit, _imageId, 0, GL_FALSE, 0, GL_READ_WRITE, _type);
}
void ImageRAM::useR(GLuint unit) {
    glBindImageTexture(unit, _imageId, 0, GL_FALSE, 0, GL_READ_ONLY, _type);
}
void ImageRAM::useW(GLuint unit) {
    glBindImageTexture(unit, _imageId, 0, GL_FALSE, 0, GL_WRITE_ONLY, _type);
}

void ImageRAM::sync() {
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
