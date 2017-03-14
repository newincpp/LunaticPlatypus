import std.container : SList;
import std.functional;
import core.stdc.stdio;

nelo ne;
extern(C) {
    alias TickFunType = void function(float);
    void init() {
	printf("init\n");
	ne.__ctor(); // work
	printf("X\n");
	ne.ninit(); // segV
	printf("init sucess!\n");
    }
    TickFunType getTickFun() {
	return ne.ngetTickFun();
    }
    uint getRemainingTickFunSize() {
	return ne.ngetRemainingTickFunSize();
    }
    void destroy() {
	ne.ndestroy();
    }
}

class nelo {
    this() {
	printf("ctor\n");
    }
    ~this() {
	printf("dtor\n");
    }
    void ninit() {
	printf("destroy D style\n");
    }
    TickFunType ngetTickFun() {
	return function void(float) { printf("doing something from the so"); };
    }
    uint ngetRemainingTickFunSize() {
	return 1;
    }
    void ndestroy() {
	printf("init D style\n");
    }
}

shared static this()
{
    printf("-------------------------------\n");
}

shared static ~this()
{
    printf("===============================\n");
}
