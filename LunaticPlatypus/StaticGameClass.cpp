#include "StaticGameClass.hh"

std::map<std::string, std::function<void(void)>> StaticGameClass::_ctorMap;
std::list<void*> StaticGameClass::_allocated;
void StaticGameClass::addImpl(std::string& name_, std::function<void(void)>& ctor_) {
    _ctorMap[name_] = ctor_;
}

bool StaticGameClass::gen(const std::string& name_) {
    decltype(_ctorMap)::iterator ctor = _ctorMap.find(name_);
    if(ctor != _ctorMap.end()) {
        ctor->second();
        return true;
    }
    return false;
}

void StaticGameClass::freeAll() {
    _allocated.clear();
}
