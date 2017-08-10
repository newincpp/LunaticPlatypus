#include "DynamicGameClass.hh"

#define LIB_NULL_PROTECT if (!_lib_handle && !isStatic) { return; }
#define autoBuildGameFunc(name) _handle.name = _buildFunction<decltype(_handle.name)>(#name)

DynamicGameClass::PlatyGameClass::PlatyGameClass() : init(nullptr), getRemainingTickFunSize(nullptr), destroy(nullptr) {}
bool DynamicGameClass::PlatyGameClass::checkInit() { return (init && getRemainingTickFunSize && destroy); }

DynamicGameClass::DynamicGameClass(std::string&& name_, Node& n_) : DynamicGameClass(name_, n_) {}

DynamicGameClass::DynamicGameClass(const std::string& name_, Node& n_) : _lib_handle(nullptr), isStatic(false) {
    if (name_.find("Static") == 0) {
        std::cout << "Static Class detected\n";
        isStatic = true;
    } else {
        _dynamicLoading(std::string("./" + name_ + ".gameClass/" + name_ + ".so"), name_, n_);
    }
}

void DynamicGameClass::_dynamicLoading(std::string&& libname, const std::string& name_, Node& n_) {
    _lib_handle = dlopen(libname.c_str(), RTLD_NOW);
    if (!_lib_handle) {
	std::cout << "failed to open \"" << libname << "\"\n";
	std::cout << "dlerror: " << dlerror() << '\n';
	return;
    }

    autoBuildGameFunc(init);
    autoBuildGameFunc(getTickFun);
    autoBuildGameFunc(getRemainingTickFunSize);
    autoBuildGameFunc(destroy);

    if (!_handle.checkInit()) {
	std::cout << "failed to reconstruct the gameClass: " << name_ << '\n';
	return;
    } else {
	std::cout << "gameClass: " << name_ << " successfully reconstructed\n";
    }
    _handle.init(&n_);
    _tickFunctions.emplace_back(_handle.getTickFun());
}

//void DynamicGameClass::update(float deltaTime_) {
//    LIB_NULL_PROTECT
//        GameClass::update(deltaTime_);
//for(decltype(_tickFunctions)::value_type f : _tickFunctions) {
//    f(deltaTime_);
//}
//}

void DynamicGameClass::reset() {
    LIB_NULL_PROTECT
	_handle.destroy();
    _tickFunctions.clear();
    _handle.init(nullptr);
}

DynamicGameClass::~DynamicGameClass() {
    LIB_NULL_PROTECT
	_handle.destroy();
    dlclose(_lib_handle);
}

void GameClass::update(float deltaTime_) {
    for(decltype(_tickFunctions)::value_type f : _tickFunctions) {
        f(deltaTime_);
    }
}

void GameClass::reset() {
}
