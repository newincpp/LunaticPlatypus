#include "OglCore.hh"
#include "GLFW//glfw3.h"
#include "node.hh"

class WindowHandle {
    public:
	GLFWwindow* _window;
	WindowHandle();
	bool exec();
	~WindowHandle();
};

class GameLogic {
    public:
	void update(){};
};

class Heart {
    private:
	WindowHandle _win;
	OglCore _renderer;
	Graph _scene;
	GameLogic _game;
    public:
	Heart();
	void run() {
	    while (_win.exec()) {
		_renderer.render();
		_game.update();
		_scene.update();
	    }
	}
};
