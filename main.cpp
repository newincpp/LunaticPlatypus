#include <iostream>
#include "RenderSystem.hh"
#include "Camera.hh"
#include "Object.hh"

int main() {
    GLnewin::IRendertarget* r = new GLnewin::Window(1920, 1080, false, "demo");
    GLnewin::Object tri;
    r->pushRenderCandidate(&tri);
    GLnewin::Camera cam = tri.genCamera();
    while (true) {
	r->render();
    }
    delete r;
}
