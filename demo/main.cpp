#include <PlatypusHeart.hh>
#include <iostream>
#include <chrono>

class MyGame : public Heart::IGamelogic {
    public:
	std::chrono::time_point<std::chrono::system_clock> start;
	glm::vec3 p;
	MyGame() {
	    _scene = "./noblesse.obj";
	    start = std::chrono::system_clock::now();
	}
	virtual void update() {
	    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
	    glm::vec3 start(0.0, 4.0, 15.3);
	    glm::vec3 end(-13.8, 4.0, 15.3);
	    
	    _lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::mix(start, end, sin(elapsed_seconds.count())));
	}
};

SetGameClass(MyGame)
