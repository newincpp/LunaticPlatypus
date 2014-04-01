#include <iostream>
#include <RenderSystem.hh>
#include <Scene.hh>

int main() {
    float x = 0;
    GLnewin::Window w(1920, 1080, false, "demo");
    GLnewin::Scene sc;
    GLnewin::Mesh tri = {
	0.0,  0.5, 1.0,
	0.5, -0.5, 1.0,
	-0.5, -0.5, 1.0 };

    sc.pushRenderCandidate(sc.genMesh("models/tri.dae"));
    GLnewin::Camera& cam = sc.getCamera();

    while (x > -30) {
	w.clear();
	sc.render();
	x -= 0.1;
	cam.position(glm::vec3(0,0,x));
	w.flush();
    }
}
