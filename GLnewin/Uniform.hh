#ifndef UNIFORM_H_
# define UNIFORM_H_
#include <iostream>
#include <string>
#include "GL.h"
#include "glm/glm.hpp"

namespace GLnewin {
    template <typename T>
	class Uniform {
	    private:
		T _value;
		GLint _location;
	    public:
		Uniform(T value, GLuint prgm, const std::string& name) : _value(value), _location(glGetUniformLocation(prgm, name.c_str())) {
		    if (_location == -1) {
			std::cout << "failed to bind location of uniform: \"" << name << "\"" << std::endl;
		    }
		}
		Uniform(T value) : _value(value) {}
		Uniform(const Uniform&) = default;
		Uniform(Uniform&&) = default;
		virtual ~Uniform() = default;
		const Uniform<T>& operator=(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator*(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator/(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator+(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator-(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		void operator+=(const Uniform<T>& v) noexcept { _value += v._value; return *this; }
		void operator-=(const Uniform<T>& v) noexcept { _value -= v._value; return *this; }
		const Uniform<T>& operator++() noexcept { ++_value; }
		const Uniform<T>& operator--() noexcept { ++_value; }
		bool operator==(const Uniform<T>& v) { return _value == v._value; }
		bool operator!=(const Uniform<T>& v) { return _value != v._value; }
		bool operator<(const Uniform<T>& v) { return _value < v._value; }
		bool operator>(const Uniform<T>& v) { return _value > v._value; }
		bool operator<=(const Uniform<T>& v) { return _value <= v._value; }
		bool operator>=(const Uniform<T>& v) { return _value >= v._value; }

		const Uniform<T>& operator=(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator*(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator/(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator+(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator-(const T&& v) noexcept { _value = v; return *this; }
		void operator+=(const T&& v) noexcept { _value += v; }
		void operator-=(const T&& v) noexcept { _value -= v; }
		bool operator==(const T&& v) noexcept { return _value == v; }
		bool operator!=(const T&& v) noexcept { return _value != v; }
		bool operator<(const T&& v) noexcept{ return _value < v; }
		bool operator>(const T&& v) noexcept{ return _value > v; }
		bool operator<=(const T&& v) noexcept { return _value <= v; }
		bool operator>=(const T&& v) noexcept { return _value >= v; }

		T& operator>>(T&& v) noexcept { v = _value; return _value; }
		T& get() noexcept { return _value; }

		virtual void upload() const noexcept;
	};
}

#endif /* !UNIFORM_H_ */
