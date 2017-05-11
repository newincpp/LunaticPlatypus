#include "DynamicGameClass.hh"


#define LIB_NULL_PROTECT if (!_lib_handle) { return; }
#define autoBuildGameFunc(name) _handle.name = _buildFunction<decltype(_handle.name)>(#name)

DynamicGameClass::PlatyGameClass::PlatyGameClass() : init(nullptr), getRemainingTickFunSize(nullptr), destroy(nullptr) {}
bool DynamicGameClass::PlatyGameClass::checkInit() { return (init && getRemainingTickFunSize && destroy); }

DynamicGameClass::DynamicGameClass(std::string&& name) : DynamicGameClass(name) {}

DynamicGameClass::DynamicGameClass(const std::string& name) : _lib_handle(nullptr) {
    std::string libname("./" + name + ".gameClass/" + name + ".so");
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
    _handle.init();
    std::cout << "test: " << _handle.getRemainingTickFunSize() << '\n';
    _tickFunctions.emplace_back(_handle.getTickFun());
}

void DynamicGameClass::update(float deltaTime_) {
    LIB_NULL_PROTECT
	for(decltype(_tickFunctions)::value_type f : _tickFunctions) {
	    f(deltaTime_);
	}
}

void DynamicGameClass::reset() {
    LIB_NULL_PROTECT
	_handle.destroy();
    _tickFunctions.clear();
    _handle.init();
}

DynamicGameClass::~DynamicGameClass() {
    LIB_NULL_PROTECT
	_handle.destroy();
    dlclose(_lib_handle);
}
