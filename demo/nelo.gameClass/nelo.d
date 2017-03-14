import std.container : SList;
import std.functional;
import core.stdc.stdio;

alias LPFun = void function(float);
extern(C++) interface PlatyInterface {
	//void init(std::list<std::function<void(float)>>* tickFunctions_);
	void init(SList!LPFun*);
	//void init();
	void destroy();
    }

extern(C++) class nelo : PlatyInterface {
    this() {
	printf("ctor\n");
    }
    ~this() {
	printf("dtor\n");
    }
    extern(C++) override void init(SList!LPFun*) {
    //extern(C++) override void init() {
	printf("destroy D style\n");
	//neloDInit();
    }
    extern(C++) override void destroy() {
	printf("init D style\n");
	//neloDDestroy();
    }
}

import std.c.stdlib;
T* create(T, Args...)(Args args)
{
	ClassInfo ci = T.classinfo;
	T* f;
	void *p;
	p = std.c.stdlib.malloc(ci.init.length);
	if (!p) {
	    return f;
	}
	// Initialize it
	(cast(byte*)p)[0 .. ci.init.length] = ci.init[];
	f = cast(T*)p;
	// Run constructor on it
	f.__ctor(args);
	return f;
}

extern (C) PlatyInterface* genClass() {
    printf("here is nelo D version\n");
    //nelo n = new nelo();
    nelo* n = create!nelo();
    printf("creating done\n");
    return cast(PlatyInterface*)n;
}

shared static this() {
    printf("libdll.so shared static this\n");
}

shared static ~this() {
    printf("libdll.so shared static ~this\n");
}
