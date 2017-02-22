#include <PlatypusHeart.hh>
#include <iostream>
#include <chrono>

class MyGame : public Heart::IGamelogic {
    public:
	std::chrono::time_point<std::chrono::system_clock> start;
	bool done;
	MyGame() : done(false) {
	    _scene = "./noblesse.obj";
	    start = std::chrono::system_clock::now();
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
