#include "RenderTexture.hh"
#include "OglCore.hh"

template <>
GLenum RenderTexture<DEPTH>::_genttachmentEnum(unsigned short) {
    return GL_DEPTH_ATTACHMENT;
}

template <>
GLenum RenderTexture<COLOR>::_genttachmentEnum(unsigned short attachment_) {
    if (GL_MAX_COLOR_ATTACHMENTS < attachment_) {
	std::cout << "max color attachement reached\n";
    }
    return attachment_ + GL_COLOR_ATTACHMENT0;
}

template <>
void RenderTexture<DEPTH>::_TexImage2D(glm::vec2& resolution_) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, resolution_.x, resolution_.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

template <>
void RenderTexture<COLOR>::_TexImage2D(glm::vec2& resolution_) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, resolution_.x, resolution_.y, 0, GL_RGB, GL_FLOAT, NULL);
}
