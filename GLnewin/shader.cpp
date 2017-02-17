#include "shader.hh"

Shader::Shader() : _vertexId(std::make_pair("", 0)), _fragmentId(std::make_pair("", 0)), _geometryId(std::make_pair("", 0)), _programId(0) {
}

void Shader::add(std::string sourceFile_, GLenum type_) {
    std::ifstream t(sourceFile_);
    std::string str((std::istreambuf_iterator<char>(t)),
	    std::istreambuf_iterator<char>());
    const char* s = str.c_str();

    GLuint* id;
    if (type_ == GL_VERTEX_SHADER) {
	id = &_vertexId.second;
	_vertexId.first = sourceFile_;
    } else if (type_ == GL_FRAGMENT_SHADER) {
	id = &_fragmentId.second;
	_fragmentId.first = sourceFile_;
    } else {
	id = &_geometryId.second;
	_geometryId.first = sourceFile_;
    }
    *id = glCreateShader(type_);
    glShaderSource(*id, 1, &s, NULL);
    glCompileShader(*id);
    _checkValidity(*id, s, sourceFile_);
}

void Shader::link(const std::vector<std::string>&& fragDataOutPut_) {
    _programId = glCreateProgram();
    if (_vertexId.second) {
	glAttachShader(_programId, _vertexId.second);
    }
    if (_fragmentId.second) {
	glAttachShader(_programId, _fragmentId.second);
    }
    if (_geometryId.second) {
	glAttachShader(_programId, _geometryId.second);
    }

    if (fragDataOutPut_.size() > GL_MAX_DRAW_BUFFERS) {
	std::cout << "max draw output exeeded! Max=" << GL_MAX_DRAW_BUFFERS << '\n';
    }
    unsigned int i = 0;
    for (const std::string& output : fragDataOutPut_) {
	glBindFragDataLocation(_programId, i, output.c_str());
	++i;
    }
    glLinkProgram(_programId);

    GLint linkStatus;
    glGetProgramiv(_programId, GL_LINK_STATUS, &linkStatus);
    if (!linkStatus) {
	std::cout << "failed to Link the shader\n";
	GLint InfoLogLength;
	glGetProgramiv(_programId, GL_INFO_LOG_LENGTH, &InfoLogLength);
	char ErrorMessage[InfoLogLength];
	glGetProgramInfoLog(_programId, InfoLogLength, NULL, ErrorMessage);
	std::cout << "\033[31mfailed to Link with error:\"" << ErrorMessage << std::endl << "-------------------\033[0m" << std::endl;
    }
    glUseProgram(_programId);
}

template <typename T>
void relocateUniform(Uniform<T>&&);
