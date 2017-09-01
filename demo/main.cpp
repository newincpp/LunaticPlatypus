#include <PlatypusHeart.hh>
#include <iostream>
#include <chrono>

#include "StaticClassTest.hh"

// This class represent your entiere application
class MyGame : public Heart::IGamelogic {
    public:
	std::chrono::time_point<std::chrono::system_clock> _start;
	glm::vec3 p;
	MyGame() {
	    //_scene = "./sponza.abc";
	    //_scene = "./vega.abc";
	    //_scene = "./Duck.gltf";
	    //_scene = "./sponza.gltf";
	    //_scene = "./sponza.glb";
            _scene = "./vega.glb";
            //_scene = "2CylinderEngine.gltf";

	    _start = std::chrono::system_clock::now();

	    _lunaticPlatypus->_game->_event.bind(std::to_string(GLFW_KEY_Q), [this]() { _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().moveUp(0.1f); }); //Q
	    _lunaticPlatypus->_game->_event.bind(std::to_string(GLFW_KEY_W), [this]() { _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().moveForward(0.1f); }); //W
	    _lunaticPlatypus->_game->_event.bind(std::to_string(GLFW_KEY_E), [this]() { _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().moveUp(-0.1f); }); //E
	    _lunaticPlatypus->_game->_event.bind(std::to_string(GLFW_KEY_A), [this]() { _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().moveRight(0.1f); }); //A
	    _lunaticPlatypus->_game->_event.bind(std::to_string(GLFW_KEY_S), [this]() { _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().moveForward(-0.1f); }); //S
	    _lunaticPlatypus->_game->_event.bind(std::to_string(GLFW_KEY_D), [this]() { _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().moveRight(-0.1f); }); //D

	    _lunaticPlatypus->_game->_event.bind("mouse", [this]() { glm::vec2 v = WindowHandle::getMouseMovement() * 0.005f; _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().rotate(glm::vec3(v.y, v.x, 0.0f)); });
	}
	// if you want to call a function after the initialisation of the engine use this function
	virtual void postEngineInit() {
            //_lunaticPlatypus->getRenderThread().uniqueTasks.push_back([this](){
		    // calling "unsafeGetRenderer" inside a "uniqueTask" make it safe because it will execute it inside the render thread
                    //std::cout << "unique task from game" << std::endl;
                    //_lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().lookAt(glm::vec3(-1.0, 13.5, 0.0));
                    //_lunaticPlatypus->getRenderThread().unsafeGetRenderer().getCamera().setPos(glm::vec3(17.5, 4, -7.7));
                    //});

            _lunaticPlatypus->getRenderThread().uniqueTasks.push_back([this](){
                    _lunaticPlatypus->getRenderThread().unsafeGetRenderer().getBuffers().emplace_back(GL_RGBA8, glm::vec3(256, 256, 256));
                    });
	}
	virtual void update() {
	    std::chrono::duration<double> elapsed_seconds = std::chrono::system_clock::now() - _start;
	    //glm::vec3 start(8.0, 1, -1.7);
	    //glm::vec3 end(8, 4.5, 1);
	    //glm::vec3 start(0.0, 4.0, 15.3);
	    //glm::vec3 end(-9.8, 7.0, 5.3);

	    //glm::vec3 start(-9.0, 27, 9.0);
	    //glm::vec3 end(17.5, 4, -7.7);
	    //glm::vec3 end(2.0, 4, -1.7);
	    //_lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::mix(start, end, sin(elapsed_seconds.count() / 5) / 2.0f + 0.5f ));
	    
	    //_lunaticPlatypus->getRenderer().getDrawBuffer()._cameras[0].setPos(glm::vec3(8.0, 2, -1.7));
	}
};

SetGameClass(MyGame)

void __Platy_FillMap__() {
     STATIC_CLASS_ADD(Vega);
}

IntrospectionDBFunc(__Platy_FillMap__);
