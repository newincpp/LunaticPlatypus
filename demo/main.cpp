#include <PlatypusHeart.hh>
#include <iostream>
#include <chrono>

// This class represent your entiere application
class MyGame : public Heart::IGamelogic {
    public:
	std::chrono::time_point<std::chrono::system_clock> start;
	bool done;
	// Your class is static so this constructor will be initialised before LunaticPlatypus Engine
	// the constructor is mostly used to pass parameters to LunaticPlatipus
	MyGame() : done(false) {
	    _scene = "./noblesse.obj";
	    start = std::chrono::system_clock::now();
	}
	// if you want to call a function after the initialisation of the engine use this function
	virtual void postEngineInit() {
	    //_lunaticPlatypus->getRenderer().getCamera().setPos(glm::vec3(9.0,3.0,0.0));
	    //_lunaticPlatypus->getRenderer().getCamera().lookAt(glm::vec3(7.0,2.5,0.0));
	}
	virtual void update() {
	    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - start;
	    //std::cout << elapsed_seconds.count() << std::endl;
	    /*
	    if ((unsigned int)elapsed_seconds.count() == 3 && !done) {
		_scene = "dieDrache.obj";
		_lunaticPlatypus->loadScene();
		std::cout << "YAY" << std::endl;
		done = true;
	    }*/
	}
};

SetGameClass(MyGame)
