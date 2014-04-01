#include <iostream>
#include <RenderSystem.hh>
#include <Scene.hh>
#include <Texture.hh>

int main() {
    float x = 20;
    GLnewin::Window w(1920, 1080, false, "demo");
    GLnewin::Scene sc;
    GLnewin::Mesh* tri;
    sc.pushRenderCandidate(tri = sc.genMesh("models/tri.dae"));
    sc.getCamera().position(glm::vec3(0,0,x));

    glm::mat4 add = glm::rotate(glm::mat4(), -1.0f, glm::vec3(0,1,0));
    glm::mat4 current = glm::mat4();
    //GLnewin::Texture<2> t;
    while (x > -30) {
	w.clear();
	sc.render();
	x -= 0.1;

	tri->refreshModifier(current);
	current *= add;
	//cam.position(glm::vec3(0,0,x));
	w.flush();
    }
}
