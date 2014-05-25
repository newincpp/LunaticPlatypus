#include <iostream>
#include <RenderSystem.hh>
#include <Scene.hh>
#include <Texture.hh>

int main(int ac, char** av) {
    float x = 20;
    GLnewin::Window w(1920, 1080, false, "demo");
    GLnewin::Scene<GLnewin::IRenderable> sc;
    GLnewin::Mesh<GL_TRIANGLES>* tri;

    GLnewin::Mesh<GL_TRIANGLES> q = {
          -1, -1, 0,
          1, 1, 0,
          -1, 1, 0,
          1, -1, 0,
          -1, -1, 0,
          1, 1, 0
    };

    if (ac > 1) {
	tri = sc.genMesh(av[1]);
    } else {
	tri = &q;
	sc.bindMatrix(q);
    }
    sc.pushRenderCandidate(tri);
    sc.getCamera().position(glm::vec3(0,0,x));
    sc.getCamera().syncronizeRez(w);

    glm::mat4 add = glm::rotate(glm::mat4(), -1.0f, glm::vec3(0,1,0));
    glm::mat4 current = glm::mat4();
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
