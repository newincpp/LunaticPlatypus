#include <vector>
#include <string>
#include "Shader.hh"

GLnewin::Shader::Shader() : _program(glCreateProgram()) {
}

void GLnewin::Shader::_coutCompileError(GLint subject) {
    GLint InfoLogLength;
    glGetShaderiv(subject, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char ErrorMessage[InfoLogLength];
    glGetShaderInfoLog(subject, InfoLogLength, NULL, ErrorMessage);
    std::cout << "error log: " << std::endl << ErrorMessage << std::endl << "-------------------" << std::endl;
}

void GLnewin::Shader::_coutLinkError(GLint subject) {
    GLint InfoLogLength;
    glGetProgramiv(subject, GL_INFO_LOG_LENGTH, &InfoLogLength);
    char ErrorMessage[InfoLogLength];
    glGetProgramInfoLog(subject, InfoLogLength, NULL, ErrorMessage);
    std::cout << "error log: " << std::endl << ErrorMessage << std::endl << "-------------------" << std::endl;
}

GLuint GLnewin::Shader::_makeShader(GLenum type, const char* source, GLint length) noexcept {
    GLuint shader;
    GLint compileStatus;

    shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    glCompileShader(shader);
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
	std::cout << "\e[1;31mfailed to compile\e[0m" << std::endl;
	_coutCompileError(shader);
	glDeleteShader(shader);
	return 0;
    } else {
	glAttachShader(_program, shader);
	std::cout << "\e[1;32mshader compiled with success\e[0m" << std::endl;
    }
    return shader;
}

void GLnewin::Shader::link() noexcept {
    GLint linkStatus;

    glLinkProgram(_program);
    glGetProgramiv(_program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
	std::cout << "\e[1;31mprogramm failed to link the program\e[0m" << std::endl;
	_coutLinkError(_program);
	glDeleteProgram(_program);
	_program = glCreateProgram();
    } else {
	std::cout << "\e[1;32mprogramm linked with success\e[0m" << std::endl;
    }
}

void GLnewin::Shader::setTesselation(const std::string& source) noexcept {
    std::cout << "compiling Tesselation shader" << std::endl;
    _tesselation = _makeShader(GL_TESS_CONTROL_SHADER, source.c_str(), source.size());
}

void GLnewin::Shader::setGeometry(const std::string& source) noexcept {
    std::cout << "compiling Geometry shader" << std::endl;
    _geometry = _makeShader(GL_GEOMETRY_SHADER, source.c_str(), source.size());
}

void GLnewin::Shader::setVertex(const std::string& source) noexcept {
    std::cout << "compiling Vertex shader" << std::endl;
    _vertex = _makeShader(GL_VERTEX_SHADER, source.c_str(), source.size());
}

void GLnewin::Shader::setFragment(const std::string& source) noexcept {
    std::cout << "compiling Fragment shader" << std::endl;
    _fragment = _makeShader(GL_FRAGMENT_SHADER, source.c_str(), source.size());
}

void GLnewin::Shader::use() noexcept {
    glUseProgram(_program);
}

template <typename T>
void GLnewin::Shader::setUniform(const Uniform<T>& v) const noexcept {
    v.upload();
}

GLnewin::Shader::~Shader() {
    glDeleteProgram(_program);
}
