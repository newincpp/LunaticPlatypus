#ifndef UNIFORM_H_
# define UNIFORM_H_
#include <string>
#include "GL.h"
#include "glm/glm.hpp"

namespace GLnewin {
    class IUniform {
	public:
	    virtual void upload() const noexcept = 0;
    };
    template <typename T>
	class Uniform : public IUniform {
	    private:
		T _value;
		GLint _location;
	    public:
		Uniform(T value, GLuint prgm, const std::string& name) : _value(value), _location(glGetUniformLocation(prgm, name.c_str())) {  }
		Uniform(T value) : _value(value) {}
		Uniform(const Uniform&) = default;
		Uniform(Uniform&&) = default;
		~Uniform() = default;
		const Uniform<T>& operator=(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator*(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator/(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator+(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		const Uniform<T>& operator-(const Uniform<T>& v) noexcept { _value = v._value; return *this; }
		void operator+=(const Uniform<T>& v) noexcept { _value += v._value; return *this; }
		void operator-=(const Uniform<T>& v) noexcept { _value -= v._value; return *this; }
		const Uniform<T>& operator++() noexcept { ++_value; }
		const Uniform<T>& operator--() noexcept { ++_value; }

		const Uniform<T>& operator=(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator*(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator/(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator+(const T&& v) noexcept { _value = v; return *this; }
		const Uniform<T>& operator-(const T&& v) noexcept { _value = v; return *this; }
		void operator+=(const T&& v) noexcept { _value += v; }
		void operator-=(const T&& v) noexcept { _value -= v; }

		virtual void upload() const noexcept;
	};
}

#endif /* !UNIFORM_H_ */
