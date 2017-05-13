#pragma once
#include "OglCore.hh"
#include "WindowHandle.hh"
#include "Node.hh"
#include "fileWatcher.hh"
#include "EventCore.hh"
#include "DynamicGameClass.hh"

#include "imgui/imgui.h"

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

class Heart {
    public:
	class IGamelogic {
	    public:
		Heart* _lunaticPlatypus;
		EventInterface _event;
		std::string _scene;
		std::list<GameClass> _gameClasses;
		virtual void postEngineInit();
		virtual void update() = 0;
	};
    private:
	WindowHandle _win;
	OglCore _renderer;
	static Graph* _scene;
	FileWatcher* _fw;
    public:
	static IGamelogic* _game;
	Heart();
	inline OglCore& getRenderer() { return _renderer; }
	static Graph* getScene() { return _scene; }
	void loadScene();
	void run();
};
#define SetGameClass(GameClass) Heart::IGamelogic* Heart::_game = new GameClass();