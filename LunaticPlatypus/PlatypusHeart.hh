#pragma once
#include "RenderThread.hh"
#include "WindowHandle.hh"
#include "Node.hh"
#include "fileWatcher.hh"
#include "EventCore.hh"
#include "DynamicGameClass.hh"

#ifdef IMGUIENABLED
#include "imgui/imgui.h"
#endif

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)

class Heart {
    public:
	class IGamelogic {
	    public:
		Heart* _lunaticPlatypus;
		EventInterface _event;
		std::string _scene;
		std::list<GameClass*> _gameClasses;
		virtual void postEngineInit();
		virtual void update() = 0;
	};
    private:
	FileWatcher* _fw;
	WindowHandle _win;
	RenderThread _renderThread;
	static Graph* _scene;
    public:
	static IGamelogic* _game;
        static void (*_fillMap)();
	Heart();
	//inline OglCore& getRenderer() { return _renderThread.unsafeGetRenderer(); }
	inline RenderThread& getRenderThread() { return _renderThread; }
	static Graph* getScene() { return _scene; }
	void loadScene();
	void addScene(std::string);
	void run();
};
#define SetGameClass(GameClass) Heart::IGamelogic* Heart::_game = new GameClass();

//void __Platy_FillMap__() {
//     STATIC_CLASS_ADD(className);
//}
#define IntrospectionDBFunc(Func) void (*Heart::_fillMap)() = &Func;
