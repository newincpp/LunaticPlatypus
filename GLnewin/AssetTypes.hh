#pragma once
#include "Renderable.hh"
#include "Mesh.hh"
#include "shader.hh"

class Asset3D {
    public:
	Mesh _mesh;
	Shader* _shader;

	//Asset3D() : _shader(nullptr) { }
	Asset3D(Mesh&& m_, Shader* s_) : _mesh(m_) , _shader(s_) { }
	Asset3D(Mesh& m_, Shader* s_) : _mesh(m_) , _shader(s_) { }
	Asset3D(Shader* s_) : _shader(s_) { }
	void update() {
	    if (_shader) {
		_shader->use();
	    } else {
		std::cout << "fck\n";
	    }
	    _mesh.render();
	}
};
