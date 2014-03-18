#include "glm/gtc/type_ptr.hpp"
#include "Uniform.hh"

//VECTORS

template <>
void GLnewin::Uniform<float>::upload() const noexcept  {
    glUniform1f(_location, _value);
}

template <>
void GLnewin::Uniform<glm::vec2>::upload() const noexcept  {
    glUniform2f(_location, _value.x, _value.y);
}

template <>
void GLnewin::Uniform<glm::vec3>::upload() const noexcept  {
    glUniform3f(_location, _value.x, _value.y, _value.z);
}

template <>
void GLnewin::Uniform<glm::vec4>::upload() const noexcept  {
    glUniform4f(_location, _value.x, _value.y, _value.z, _value.w);
}

//MATRIX

template <>
void GLnewin::Uniform<glm::mat2>::upload() const noexcept  {
     glUniformMatrix2fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat3>::upload() const noexcept  {
  glUniformMatrix3fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat4>::upload() const noexcept  {
  glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_value));
}

template <>
void GLnewin::Uniform<glm::mat2x3>::upload() const noexcept  {
  glUniformMatrix2x3fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat3x2>::upload() const noexcept  {
  glUniformMatrix3x2fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat2x4>::upload() const noexcept  {
  glUniformMatrix2x4fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat4x2>::upload() const noexcept  {
  glUniformMatrix4x2fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat3x4>::upload() const noexcept  {
  glUniformMatrix3x4fv(_location, 1, GL_FALSE, &_value[0][0]);
}

template <>
void GLnewin::Uniform<glm::mat4x3>::upload() const noexcept  {
  glUniformMatrix4x3fv(_location, 1, GL_FALSE, &_value[0][0]);
}
