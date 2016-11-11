#include "RenderTexture.hh"
#include "OglCore.hh"

template <>
GLenum RenderTexture<GL_DEPTH_COMPONENT>::_genttachmentEnum(unsigned short) {
    return GL_DEPTH_ATTACHMENT;
}

template <>
GLenum RenderTexture<GL_RGB>::_genttachmentEnum(unsigned short attachment_) {
    if (GL_MAX_COLOR_ATTACHMENTS < attachment_) {
	std::cout << "max color attachement reached\n";
    }
    return attachment_ + GL_COLOR_ATTACHMENT0;
}
