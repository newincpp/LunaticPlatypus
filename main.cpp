#include <iostream>
#include "RenderSystem.hh"
#include "Mesh.hh"

int main() {
    GLnewin::IRendertarget* r = new GLnewin::Window(100, 100, false, "demo");
    r->pushRenderCandidate(new GLnewin::Mesh({-1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f}));
    while (true) {
	r->render();
    }
    delete r;
}
