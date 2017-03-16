#include <iostream>
#include <PlatyInterface>
#include "nelo.hh"

nelo::nelo() : functionToPush(1) {
    std::cout << "let's create life\n";
}

void(*nelo::getTickFun())(float) {
    return [](float){ std::cout << "doing something...\n"; };
}

unsigned int nelo::getRemainingTickFunSize() {
    return 1;
}

void nelo::init() {
    std::cout << "I am Nelo\n";
}

void nelo::destroy() {
    std::cout << "aaaaaannnd am ded =(\n";
}

nelo::~nelo() {
    std::cout << "truely destroyed\n";
}

nelo g_nelo;
void init() { g_nelo.init(); }
void(*getTickFun())(float) { return g_nelo.getTickFun(); }
unsigned int getRemainingTickFunSize() { return g_nelo.getRemainingTickFunSize(); }
void destroy() { g_nelo.destroy(); }
