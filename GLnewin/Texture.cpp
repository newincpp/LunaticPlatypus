#include "Texture.hh"

Texture::Texture() {
    glGenTextures(1, &_textureID);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void Texture::load(const std::string&) {
}

void Texture::load(float* buffer_, unsigned int w_, unsigned int h_) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, w_, h_, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer_);
}

void Texture::load() {
    float buffer[4] =
    {
	1.0, 0.0,
	0.0, 1.0,
    };
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _textureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 2, 2, 0, GL_BGR, GL_UNSIGNED_BYTE, buffer);
}
