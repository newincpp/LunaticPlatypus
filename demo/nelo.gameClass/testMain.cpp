#include <dlfcn.h>
#include <iostream>
#include <PlatyInterface>

#define LIB_NULL_PROTECT if (!_lib_handle) { return; }

class DynamicGameClass {
    private:
	void* _lib_handle;
	PlatyInterface* _handle;
	PlatyInterface* (*_genClass)();
	std::list<std::function<void(float)>> _tickFunctions;
    public:
	DynamicGameClass(std::string&& name) : DynamicGameClass(name) {}
	DynamicGameClass(std::string& name) : _lib_handle(nullptr), _handle(nullptr), _genClass(nullptr) {
	    std::string libname("./" + name + ".so");
	    _lib_handle = dlopen(libname.c_str(), RTLD_NOW);
	    if (!_lib_handle) {
		std::cout << "failed to open \"" << libname << "\"\n";
		return;
	    }
	    std::cout << libname << '\n';

	    _genClass = (PlatyInterface* (*)())dlsym(_lib_handle, "genClass");
	    char *error;
	    if ((error = dlerror()) != NULL) {
		std::cout << "failed to find genClass function:\n" << error << '\n';
		return;
	    }
	    std::cout << "getting genClass func ok\n";
	    _handle = _genClass();
	    if (!_handle) {
		std::cout << "something wrong happened\n";
	    }
	    std::cout << "genClass exec ok\n";
	    _handle->init(&_tickFunctions);
	    std::cout << "init call ok\n";
	}
	void update(float deltaTime_) {
	    LIB_NULL_PROTECT
	    for(std::function<void(float)>& f : _tickFunctions) {
		f(deltaTime_);
	    }
	}
	void reset() {
	    LIB_NULL_PROTECT
	    _handle->destroy();
	    delete _handle;
	    _handle = _genClass();
	    _tickFunctions.clear();
	    _handle->init(&_tickFunctions);
	}
	~DynamicGameClass() {
	    LIB_NULL_PROTECT
	    _handle->destroy();
	    std::cout << "destroy call ok\n";
	    delete _handle;
	    std::cout << "destructor call ok\n";
	    dlclose(_lib_handle);
	    std::cout << "handle deleted\n";
	}
};
#undef LIB_NULL_PROTECT

int main(int ac, char** av){
    std::string name("nelo");
    if (ac > 1) {
	name = av[1];
    }
    DynamicGameClass p(name);
    p.reset();
    for (unsigned short i = 0; i < 5; ++i) {
	p.update(0.0f);
    }
}
