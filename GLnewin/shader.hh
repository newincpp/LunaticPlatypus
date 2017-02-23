#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include <streambuf>
#include <utility>

#include "glew.h"

#define checkGlError getGlError(__FILE__, __LINE__);
void getGlError(const char* file_, unsigned long line_);
class Uniform;

class Shader {
    private:
	std::pair<std::string, GLuint> _vertexId;
	std::pair<std::string, GLuint> _fragmentId;
	std::pair<std::string, GLuint> _geometryId;
	GLuint _programId;
    public:
	std::vector<std::pair<Uniform &, unsigned int>> uniformList;
	std::vector<std::string> containedUniformNames;

	inline bool _checkValidity(GLenum id_, const char* src_, std::string& filename_)const {
	    GLint compileStatus;
	    glGetShaderiv(id_, GL_COMPILE_STATUS, &compileStatus);
	    if (compileStatus) {
		std::cout << "\033[32m\"" << filename_ << "\" succesfully compiled\033[0m\n";
		return true;
	    }
	    GLint InfoLogLength;
	    glGetShaderiv(id_, GL_INFO_LOG_LENGTH, &InfoLogLength);
	    char ErrorMessage[InfoLogLength];
	    glGetShaderInfoLog(id_, InfoLogLength, NULL, ErrorMessage);

	    std::cout << "\033[31mfailed to compile \"" <<  filename_ << "\"\033[0m\n" << 
		"your shader:\n" << 
		src_ << '\n' <<
		"error log:\n" << ErrorMessage << std::endl << "-------------------\033[0m\n";
	    return false;
	}

    public:
	Shader();
	void add(std::string, GLenum);
	bool containUniform(Uniform &u_);
	void link(const std::vector<std::string>&& fragDataOutPut_);
	template <typename T>
		void relocateUniform(Uniform&&, const char* name_);
	void use();
};
