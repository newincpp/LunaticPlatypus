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
	DynamicGameClass(std::string&& name);
	DynamicGameClass(std::string& name);
	void update(float deltaTime_);
	void reset();
	~DynamicGameClass();
};
