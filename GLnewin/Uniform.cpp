#include "Uniform.hh"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

template <>
void Uniform<float>::upload() {
    glUniform1f(_location, _value);
}

template <>
void Uniform<int>::upload() {
    glUniform1i(_location, _value);
}

template <>
void Uniform<unsigned int>::upload() {
    glUniform1ui(_location, _value);
}

template <>
void Uniform<glm::vec2>::upload() {
    glUniform2f(_location, _value.x, _value.y);
}

template <>
void Uniform<glm::vec3>::upload() {
    glUniform3f(_location, _value.x, _value.y, _value.z);
}

template <>
void Uniform<glm::vec4>::upload() {
    glUniform4f(_location, _value.x, _value.y, _value.z, _value.w);
}

template <>
void Uniform<glm::mat2>::upload() {
    glUniformMatrix2fv(_location, 1, GL_FALSE, glm::value_ptr(_value));
}

template <>
void Uniform<glm::mat3>::upload() {
    glUniformMatrix3fv(_location, 1, GL_FALSE, glm::value_ptr(_value));
}

template <>
void Uniform<glm::mat4>::upload() {
    glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_value));
}
