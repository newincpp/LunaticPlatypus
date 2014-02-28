#include "Object.hh"

GLnewin::Object::Object() : _mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}) {
    _shader.setVertex(GLnewin::Shader::fileToString("vert.glsl"));
    _shader.setFragment(GLnewin::Shader::fileToString("frag.glsl"));
    _shader.link();
    _mesh.setShader(&_shader);
    //GLnewin::Uniform<GLfloat> color = _shader.genUniform(0.1f, "color");
    //GLnewin::Uniform<glm::vec4> displacement = _shader.genUniform(glm::vec4(0.1,0.1,0.1, 0.1), "displacement");
    //glm::vec4& d = displacement.get();
}

void GLnewin::Object::draw() noexcept {
    _mesh.draw();
}
