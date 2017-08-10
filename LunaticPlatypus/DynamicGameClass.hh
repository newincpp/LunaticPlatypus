#pragma once
#include <dlfcn.h>
#include <iostream>
#include <list>
#include "Node.hh"

class GameClass {
    protected:
	typedef void (*TickFunType)(float);
	std::list<TickFunType> _tickFunctions;
    public:
        GameClass() = default;
        virtual void update(float deltaTime_);
	virtual void reset();
};

class DynamicGameClass : public GameClass {
    private:
	void* _lib_handle;
        bool isStatic;
	class PlatyGameClass {
	    public:
		PlatyGameClass();
		void (*init)(Node*);
                GameClass::TickFunType (*getTickFun)();
		unsigned int (*getRemainingTickFunSize)();
		void (*destroy)();
		inline bool checkInit();
	};

	template <typename T>
	T _buildFunction(const char* name) {
	    T func = (T)dlsym(_lib_handle, name);
	    char *error;
	    if ((error = dlerror()) != NULL) {
		std::cout << "failed to find genClass function:\n" << error << '\n';
	    } else if (func == nullptr){
		std::cout << name << " is still null dunno why\n";
	    }
	    return func;
	}
	PlatyGameClass _handle;
        void _dynamicLoading(std::string&&, const std::string&, Node&);
    public:
	DynamicGameClass(std::string&& name, Node&);
	DynamicGameClass(const std::string& name, Node&);
	virtual void reset();
	~DynamicGameClass();
};

//typedef DynamicGameClass GameClass; // TODO scripting support
