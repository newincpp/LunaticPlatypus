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

class IGame {
    public:
	virtual void init() = 0;
	virtual void update() = 0;
	virtual void deInit() = 0;
};

class GameLogic {
    public:
	static IGame* _endUserLogic;
	GameLogic() { if(_endUserLogic) { _endUserLogic->init(); } }
	void update() { if(_endUserLogic) {_endUserLogic->update(); } }
	~GameLogic() { if(_endUserLogic) { _endUserLogic->deInit(); } }
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

#define SetGameClass(GameClass) IGame* GameLogic::_endUserLogic = new GameClass();
