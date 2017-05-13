#include <dlfcn.h>
#include <iostream>
#include <list>

#define LIB_NULL_PROTECT if (!_lib_handle) { return; }

class DynamicGameClass {
    private:
	void* _lib_handle;
	typedef void (*TickFunType)(float);
	class PlatyGameClass {
	    public:
		PlatyGameClass() : init(nullptr), getRemainingTickFunSize(nullptr), destroy(nullptr) {}
		void (*init)(void*);
		TickFunType (*getTickFun)();
		unsigned int (*getRemainingTickFunSize)();
		void (*destroy)();
		inline bool checkInit() { return (init && getRemainingTickFunSize && destroy); }
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
#define autoBuildGameFunc(name) _handle.name = _buildFunction<decltype(_handle.name)>(#name)
	PlatyGameClass _handle;
	std::list<TickFunType> _tickFunctions;
    public:
	DynamicGameClass(std::string&& name) : DynamicGameClass(name) {}
	DynamicGameClass(std::string& name) : _lib_handle(nullptr) {
	    std::string libname("./" + name + ".so");
	    _lib_handle = dlopen(libname.c_str(), RTLD_NOW);
	    if (!_lib_handle) {
		std::cout << "failed to open \"" << libname << "\"\n";
		return;
	    }

	    autoBuildGameFunc(init);
	    autoBuildGameFunc(getTickFun);
	    autoBuildGameFunc(getRemainingTickFunSize);
	    autoBuildGameFunc(destroy);

	    if (!_handle.checkInit()) {
		std::cout << "failed to reconstruct the gameClass: " << name << '\n';
		return;
	    } else {
		std::cout << "gameClass: " << name << " successfully reconstructed\n";
	    }
	    _handle.init(nullptr);
	    std::cout << "test: " << _handle.getRemainingTickFunSize() << '\n';
	    for (unsigned int i = _handle.getRemainingTickFunSize(); i > 0 ; --i) {
		_tickFunctions.emplace_back(_handle.getTickFun());
	    }
	}
	void update(float deltaTime_) {
	    LIB_NULL_PROTECT
	    for(decltype(_tickFunctions)::value_type f : _tickFunctions) {
		f(deltaTime_);
	    }
	}
	void reset() {
	    LIB_NULL_PROTECT
	    _handle.destroy();
	    _tickFunctions.clear();
	    _handle.init(nullptr);
	}
	~DynamicGameClass() {
	    LIB_NULL_PROTECT
	    _handle.destroy();
	    dlclose(_lib_handle);
	}
};
#undef LIB_NULL_PROTECT

int main(int ac, char** av){
    std::string name("nelo");
    if (ac > 1) {
	name = av[1];
    }
    DynamicGameClass p(name);
    for (unsigned short i = 0; i < 5; ++i) {
	p.update(0.0f);
    }
    p.reset();
}
