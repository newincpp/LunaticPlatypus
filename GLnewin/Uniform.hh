#pragma once
#include <iostream>
#include "glew.h"

template <typename T>
class Uniform {
    public:
	GLint _location;
	T _value;
    	explicit Uniform(T v_) : _value(v_) {}
    	explicit Uniform(GLint l_, T v_) : _location(l_), _value(v_) {}
	operator T() { return _value; }
	void operator=(T v_) { _value = v_; }
	void upload(); 
	void _resetLocation(const char* name_) {
		GLint programId;
		glGetIntegerv(GL_CURRENT_PROGRAM, &programId);
		_location = glGetUniformLocation(programId, name_);
		std::cout << "uniform: " << name_ << " " << _location << std::endl;
	}
#define autoRelocate(var) var._resetLocation(#var);
};

