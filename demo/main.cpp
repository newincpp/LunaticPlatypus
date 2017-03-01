#include <PlatypusHeart.hh>
#include <iostream>
#include <chrono>

class MyGame : public Heart::IGamelogic {
    public:
	std::chrono::time_point<std::chrono::system_clock> _start;
	glm::vec3 p;
	MyGame() {
	    _scene = "./sponza.obj";

	    _start = std::chrono::system_clock::now();
	}
	virtual void update() {
	    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _start;
	    //glm::vec3 start(8.0, 1, -1.7);
	    //glm::vec3 end(8, 4.5, 1);
	    //glm::vec3 start(0.0, 4.0, 15.3);
	    //glm::vec3 end(-9.8, 7.0, 5.3);

	    glm::vec3 start(-5.5, 8, 3.7);
	    glm::vec3 end(8.0, 2, -1.7);
	    //glm::vec3 end(2.0, 4, -1.7);
	    _lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::mix(start, end, sin(elapsed_seconds.count() / 3) / 2.0f + 0.5f ));
	    
	    _lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].lookAt(glm::vec3(-1, 2, 0));
	    //_lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::vec3(8.0, 2, -1.7));
	}
};

SetGameClass(MyGame)
