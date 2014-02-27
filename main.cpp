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

    GLnewin::Uniform<float> value(0.01f, s.getProgram(), "color");

    while (true) {
	s.setUniform(&value);
	value += 0.001f;
	r->render();
    }
    delete r;
}
