#include <dlfcn.h>
#include <iostream>
#include <list>
//#include <PlatyInterface>

#define LIB_NULL_PROTECT if (!_lib_handle) { return; }

class DynamicGameClass {
    private:
	void* _lib_handle;
	typedef void (*TickFunType)(float);
	class PlatyGameClass {
	    public:
		PlatyGameClass() : init(nullptr), getRemainingTickFunSize(nullptr), destroy(nullptr) {}
		void (*init)();
		TickFunType (*getTickFun)();
		unsigned int (*getRemainingTickFunSize)();
		void (*destroy)();
		bool checkInit() { return (init && getRemainingTickFunSize && destroy); }
	};
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
	    std::cout << libname << '\n';

	    _handle.init = (void(*)())dlsym(_lib_handle, "init");
	    _handle.getTickFun = (TickFunType(*)())dlsym(_lib_handle, "getTickFun");
	    _handle.getRemainingTickFunSize = (unsigned int(*)())dlsym(_lib_handle, "getRemainingTickFunSize");
	    _handle.destroy = (void(*)())dlsym(_lib_handle, "destroy");

	    char *error;
	    if ((error = dlerror()) != NULL) {
		std::cout << "failed to find genClass function:\n" << error << '\n';
		return;
	    }
	    if (_handle.checkInit()) {
		std::cout << "gameClass successfully reconstructed\n";
	    }
	    std::cout << "test: " << _handle.getRemainingTickFunSize() << '\n';
	    _tickFunctions.emplace_back(_handle.getTickFun());
	}
	void update(float deltaTime_) {
	    LIB_NULL_PROTECT
	    for(decltype(_tickFunctions)::value_type f : _tickFunctions) {
		f(deltaTime_);
	    }
	}
	void reset() {
	    LIB_NULL_PROTECT
	    _tickFunctions.clear();
	}
	~DynamicGameClass() {
	    LIB_NULL_PROTECT
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
