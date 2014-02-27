#include <iostream>
#include "RenderSystem.hh"
#include "Mesh.hh"

int main() {
    GLnewin::Mesh* tri;
    GLnewin::IRendertarget* r = new GLnewin::Window(1920, 1080, false, "demo");
    r->pushRenderCandidate(tri = new GLnewin::Mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}));

    GLnewin::Shader s;
    s.setVertex(GLnewin::Shader::fileToString("vert.glsl"));
    s.setFragment(GLnewin::Shader::fileToString("frag.glsl"));
    s.link();
    tri->setShader(&s);

    GLnewin::Uniform<GLfloat> color = s.genUniform(0.1f, "color");
    GLnewin::Uniform<glm::vec4> displacement = s.genUniform(glm::vec4(0.1,0.1,0.1, 0.1), "displacement");
    glm::vec4& d = displacement.get();

    while (true) {
	if (color <= 0.5f) {
	    color += 0.01;
	    d.x += 0.01;
	} else {
	    color = 0.0f;
	    d.x = 0;
	}
	s.setUniform(&displacement);
	s.setUniform(&color);
	r->render();
    }
    delete r;
}
