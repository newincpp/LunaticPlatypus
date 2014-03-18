#include <iostream>
#include <RenderSystem.hh>
#include <Scene.hh>

int main(int ac, char** av) {
    GLnewin::IRendertarget* r = new GLnewin::Window(1920, 1080, false, "demo");
    GLnewin::Scene sc;
    GLnewin::Mesh tri(sc.genMesh("models/tri.dae"));

    r->pushRenderCandidate(&tri);
    GLnewin::Camera cam = sc.genCamera();
    cam.lookAt(glm::vec3(0.0, 0.0, 1.0));
    float x = 0;
    while (true) {
	r->render();
	sc.setActive();
	x -= 0.1;
	//tri.setPos(glm::vec3(0,0,-3));
	//sc.getCamera().position(glm::vec3(0,0,x));
    }
    delete r;
}
