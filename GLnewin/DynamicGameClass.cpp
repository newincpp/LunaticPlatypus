#include "DynamicGameClass.hh"

DynamicGameClass::DynamicGameClass(std::string&& name) : DynamicGameClass(name) {}
DynamicGameClass::DynamicGameClass(std::string& name) : _lib_handle(nullptr), _handle(nullptr), _genClass(nullptr) {
    std::string libname("./" + name + ".gameClass/" + name + ".so");
    _lib_handle = dlopen(libname.c_str(), RTLD_LAZY);
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
    _handle = _genClass();
    if (!_handle) {
	std::cout << "something wrong happened\n";
    }
    _handle->init(&_tickFunctions);
}
void DynamicGameClass::update(float deltaTime_) {
    LIB_NULL_PROTECT
	for(std::function<void(float)>& f : _tickFunctions) {
	    f(deltaTime_);
	}
}
void DynamicGameClass::reset() {
    LIB_NULL_PROTECT
	_handle->destroy();
    delete _handle;
    _handle = _genClass();
    _tickFunctions.clear();
    _handle->init(&_tickFunctions);
}
DynamicGameClass::~DynamicGameClass() {
    LIB_NULL_PROTECT
	_handle->destroy();
    delete _handle;
    dlclose(_lib_handle);
}
