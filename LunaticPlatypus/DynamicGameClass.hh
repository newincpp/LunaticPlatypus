#include <dlfcn.h>
#include <iostream>
#include <list>
#include "Node.hh"

class DynamicGameClass {
    private:
	void* _lib_handle;
	typedef void (*TickFunType)(float);
	class PlatyGameClass {
	    public:
		PlatyGameClass();
		void (*init)(Node*);
		TickFunType (*getTickFun)();
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
	std::list<TickFunType> _tickFunctions;
    public:
	DynamicGameClass(std::string&& name, Node&);
	DynamicGameClass(const std::string& name, Node&);
	void update(float deltaTime_);
	void reset();
	~DynamicGameClass();
};

typedef DynamicGameClass GameClass; // TODO scripting support
