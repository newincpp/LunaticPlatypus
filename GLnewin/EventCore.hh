#pragma once
#include <list>
#include <map>
#include <functional>

class EventCore {
    public:
	typedef std::map<std::string, std::list<std::function<void(void)>>> EventMap;
	typedef std::pair<EventCore::EventMap::iterator, std::list<std::function<void(void)>>::iterator> evRev;
    private:
	EventMap _Elist;
    public:
	void getSockets(std::list<std::string>&);
	evRev bindAction(std::string&, std::function<void(void)>);
	void unbind(EventMap::iterator&);
	void call(std::string&);
};

class EventInterface {
    private:
	static EventCore* _eCore;
	std::list<EventCore::evRev> _selfEvents;
    public:
	EventInterface();
	void exec(std::string&&);
	static void sExec(std::string&&);
	void bind(std::string&&, std::function<void()>&&);
};
