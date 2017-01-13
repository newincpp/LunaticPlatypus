#include "OglCore.hh"
#include "GLFW//glfw3.h"
#include "node.hh"
#include "fileWatcher.hh"

#include "imgui/imgui.h"

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

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
	std::string sceneFileName;
	static IGame* _endUserLogic;

	GameLogic() : sceneFileName(STRINGIZE(DEFAULT_SCENE)) { if(_endUserLogic) { _endUserLogic->init(); } }
	void update() { if(_endUserLogic) {_endUserLogic->update(); } }
	~GameLogic() { if(_endUserLogic) { _endUserLogic->deInit(); } }
};

class Heart {
    private:
	WindowHandle _win;
	OglCore _renderer;
	Graph _scene;
	GameLogic _game;

	FileWatcher* _fw;
    public:
	Heart();
	void run();
};

#define SetGameClass(GameClass) IGame* GameLogic::_endUserLogic = new GameClass();
