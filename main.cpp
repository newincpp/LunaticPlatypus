#include <iostream>
#include "RenderSystem.hh"
#include "Mesh.hh"

int main() {
    GLnewin::IRenderable* tri;
    GLnewin::IRendertarget* r = new GLnewin::Window(100, 100, false, "demo");
    r->pushRenderCandidate(tri = new GLnewin::Mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}));

    GLnewin::Shader s;
    s.setVertex(GLnewin::Shader::fileToString("vert.glsl"));
    s.setFragment(GLnewin::Shader::fileToString("frag.glsl"));
    s.link();
    tri->setShader(&s);

    while (true) {
	r->render();
    }
    delete r;
}
