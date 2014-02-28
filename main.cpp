#include <iostream>
#include "RenderSystem.hh"
#include "Camera.hh"
#include "Object.hh"

int main() {
    GLnewin::IRendertarget* r = new GLnewin::Window(1920, 1080, false, "demo");
    GLnewin::Object tri;
    r->pushRenderCandidate(&tri);
    GLnewin::Camera cam = tri.genCamera();
    cam.lookAt(glm::vec3(0.0, 0.0, 1.0));
    float x = 0;
    while (true) {
	r->render();
	x -= 0.1;
	cam.setActive();
	tri.setPos(glm::vec3(0,0,-3));
	cam.position(glm::vec3(0,0,x));
    }
    delete r;
}
