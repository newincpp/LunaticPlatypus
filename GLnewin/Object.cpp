#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include "glm/gtc/matrix_transform.hpp"
#include "Object.hh"

//http://ogldev.atspace.co.uk/www/tutorial22/tutorial22.html

GLnewin::Object::Object(const std::string& file) : _mesh({0}), _rotPosScale(NULL) {
    _shader.setVertex(GLnewin::Shader::fileToString("shaders/vert.glsl"));
    _shader.setFragment(GLnewin::Shader::fileToString("shaders/frag.glsl"));
    _shader.link();
    _mesh.setShader(&_shader);
    _rotPosScale = new Uniform<glm::mat4>(_shader.genUniform(glm::mat4(), "object"));
}

void GLnewin::Object::draw() const noexcept {
    _mesh.draw();
}

void GLnewin::Object::setPos(const glm::vec3& v) noexcept {
    _pos = v;
    *_rotPosScale = glm::translate(glm::mat4(), _pos);
    _rotPosScale->upload();
}

void GLnewin::Object::setRot(const glm::vec3& v) noexcept {
    _rot = v;
}

void GLnewin::Object::setScale(const glm::vec3& v) noexcept {
    _scale = v;
}

