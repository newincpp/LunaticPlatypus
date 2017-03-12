#include <PlatypusHeart.hh>
#include <iostream>
#include <chrono>

// This class represent your entiere application
class MyGame : public Heart::IGamelogic {
    public:
	std::chrono::time_point<std::chrono::system_clock> _start;
	glm::vec3 p;
	MyGame() {
	    _scene = "./nelo.abc";

	    _start = std::chrono::system_clock::now();
	}
	// if you want to call a function after the initialisation of the engine use this function
	virtual void postEngineInit() {
	    //_lunaticPlatypus->getRenderer().getCamera().setPos(glm::vec3(9.0,3.0,0.0));
	    _lunaticPlatypus->getRenderer().getCamera().lookAt(glm::vec3(-1.0, 13.5, 0.0));
	    _lunaticPlatypus->getRenderer().getCamera().setPos(glm::vec3(17.5, 4, -7.7));
	}
	virtual void update() {
	    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _start;
	    //glm::vec3 start(8.0, 1, -1.7);
	    //glm::vec3 end(8, 4.5, 1);
	    //glm::vec3 start(0.0, 4.0, 15.3);
	    //glm::vec3 end(-9.8, 7.0, 5.3);

	    glm::vec3 start(-9.0, 27, 9.0);
	    glm::vec3 end(17.5, 4, -7.7);
	    //glm::vec3 end(2.0, 4, -1.7);
	    _lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::mix(start, end, sin(elapsed_seconds.count() / 5) / 2.0f + 0.5f ));
	    
	    //_lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::vec3(8.0, 2, -1.7));
	}
};

SetGameClass(MyGame)
