import std.functional;
import core.stdc.stdio;
import std.c.stdlib;
T create(T, Args...)(Args args)
{		
	ClassInfo ci = T.classinfo;
	T f;
	void *p;
	p = std.c.stdlib.malloc(ci.init.length);
	if (!p) {
	    return f;
	}
	// Initialize it
	(cast(byte*)p)[0 .. ci.init.length] = ci.init[];
	f = cast(T)p;
	// Run constructor on it
	f.__ctor(args);
	return f;
}

nelo g_nelo;
extern(C) alias TickFunType = void function(float);
extern(C) void init() {
    g_nelo = create!nelo();
}
extern(C) TickFunType getTickFun() {
    return g_nelo.getTickFun();
}
extern(C) uint getRemainingTickFunSize() {
    return g_nelo.getRemainingTickFunSize();
}
extern(C) void destroy() {
    g_nelo.destroy();
}

class nelo {
    this() {
	printf("ctor\n");
    }
    ~this() {
	printf("dtor\n");
    }
    void init() {
	printf("init D style\n");
    }
    TickFunType getTickFun() {
	return function void(float) { printf("D implementation of nelo\n"); };
    }
    uint getRemainingTickFunSize() {
	return 1;
    }
    void destroy() {
	printf("destroy D style\n");
    }
}

shared static this() {
    printf("-------------------------------\n");
}

shared static ~this() {
    printf("===============================\n");
}
