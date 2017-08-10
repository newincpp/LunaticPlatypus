#include "StaticGameClass.hh"

std::map<std::string, GameClass*(*)(Node&)> StaticGameClassGenerator::_ctorMap;
void StaticGameClassGenerator::addImpl(std::string name_, GameClass*(*ctor_)(Node&)) {
    _ctorMap[name_] = ctor_;
}

GameClass* StaticGameClassGenerator::gen(const std::string& name_, Node& n_) {
    decltype(_ctorMap)::iterator ctor = _ctorMap.find(name_);
    if(ctor != _ctorMap.end()) {
        return ctor->second(n_);
    } else {
        std::cout << "failed to find as static:" << name_ << '\n';
    }
    return nullptr;
}
