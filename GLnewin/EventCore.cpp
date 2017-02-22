#include <iostream>
#include "EventCore.hh"

void EventCore::getSockets(std::list<std::string>&) {
}

EventCore::evRev EventCore::bindAction(std::string& evName_, std::function<void(void)> fun_) {
    _Elist[evName_].push_back(fun_);
    return std::make_pair(_Elist.find(evName_), _Elist[evName_].end());
}

void EventCore::unbind(EventCore::EventMap::iterator&) {
}

void EventCore::call(std::string& evName_) {
    for(auto f : _Elist[evName_]) {
	f();
    }
}

EventInterface::EventInterface() {
    if (!_eCore) {
	_eCore = new EventCore();
    }
}

void EventInterface::exec(std::string& evName_) {
    _eCore->call(evName_);
}

void EventInterface::sExec(std::string& evName_) {
    if (EventInterface::_eCore) {
	_eCore->call(evName_);
    }
}

void EventInterface::bind(std::string& evName_, std::function<void()>& fun_) {
    _eCore->bindAction(evName_, fun_);
}

void EventInterface::exec(std::string&& evName_) {
    _eCore->call(evName_);
}

void EventInterface::sExec(std::string&& evName_) {
    if (EventInterface::_eCore) {
	_eCore->call(evName_);
    }
}

void EventInterface::bind(std::string&& evName_, std::function<void()>& fun_) {
    _eCore->bindAction(evName_, fun_);
}

EventCore* EventInterface::_eCore = nullptr;
