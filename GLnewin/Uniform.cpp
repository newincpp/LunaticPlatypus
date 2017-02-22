#include <functional>
#include "Uniform.hh"

void Uniform::upload() const {
    if (_location < 0) {
	return;
    }
    //std::cout << "name= " << name << ' ';
    if (_valueType == tInt) {
	//std::cout << "upload int " << _value.i << " at location " << _location << '\n';
	glUniform1i(_location, _value.i);
    } else if (_valueType == tUnsignedInt) {
	//std::cout << "upload unsigned int " << _value.ui << " at location " << _location << '\n';
	glUniform1ui(_location, _value.ui);
    } else if (_valueType == tFloat) {
	//std::cout << "upload float " << _value.f << " at location " << _location << '\n';
	glUniform1f(_location, _value.f);
    } else if (_valueType == tVec2) {
	glUniform2f(_location, _value.v2.x, _value.v2.y);
	//std::cout << "upload vec2 " << _value.v2.x << " " << _value.v2.y << " at location " << _location << '\n';
    } else if (_valueType == tVec3) {
	//std::cout << "upload vec3 " << _value.v3.x << " " << _value.v3.y << ' ' << _value.v3.z << " at location " << _location << '\n';
	glUniform3f(_location, _value.v3.x, _value.v3.y, _value.v3.z);
    } else if (_valueType == tVec4) {
	//std::cout << "upload vec4 " << _value.v3.x << " " << _value.v3.y << ' ' << _value.v3.z << ' ' << _value.v4.w << " at location " << _location << '\n';
        glUniform4f(_location, _value.v4.x, _value.v4.y, _value.v4.z, _value.v4.w);
    } else if (_valueType == tMat2) {
	//std::cout << "upload mat2 at location " << _location << '\n';
	glUniformMatrix2fv(_location, 1, GL_FALSE, glm::value_ptr(_value.m2));
    } else if (_valueType == tMat3) {
	//std::cout << "upload mat3 at location " << _location << '\n';
        glUniformMatrix3fv(_location, 1, GL_FALSE, glm::value_ptr(_value.m3));
    } else if (_valueType == tMat4) {
	//std::cout << "upload mat4 at location " << _location << '\n';
	glUniformMatrix4fv(_location, 1, GL_FALSE, glm::value_ptr(_value.m4));
    } else {
	std::cout << "unknown uniform type\n";
    }
}

void Uniform::addItselfToShaders(std::vector<Shader> &shaderList) {
    for (std::vector<Shader>::iterator shader = shaderList.begin(); shader != shaderList.end(); shader++) {
	shader->containUniform(*this);
    }
}
