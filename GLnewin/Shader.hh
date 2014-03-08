#ifndef SHADER_H_
# define SHADER_H_
#include <iostream>
#include <fstream>
#include <streambuf>
#include "GL.h"
#include "Uniform.hh"

namespace GLnewin {
    class Shader {
	private:
	    GLuint _tesselation;
	    GLuint _geometry;
	    GLuint _vertex;
	    GLuint _fragment;
	    GLuint _program;

	    GLuint _makeShader(GLenum, const char* source, GLint length) noexcept;
	    void _coutCompileError(GLint);
	    void _coutLinkError(GLint);
	public:
	    explicit Shader();
	    ~Shader();

	    void setTesselation(const std::string& source)noexcept;
	    void setGeometry(const std::string& source)noexcept;
	    void setVertex(const std::string& source)noexcept;
	    void setFragment(const std::string& source)noexcept;
	    void link()noexcept;
	    static std::string fileToString(const std::string& file)noexcept{
		std::ifstream t(file);
		return std::string((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	    }
	    void use() noexcept ;
	    template <typename T>
		void setUniform(const Uniform<T>& value)const noexcept;
	    template<typename T>
		Uniform<T> genUniform(T value, const std::string& name) const noexcept {
		    return Uniform<T>(value, _program, name);
		}
    };
}

#endif /* !SHADER_H_ */
