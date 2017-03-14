#include <iostream>
#include "nelo.hh"

nelo::nelo() : _tickFunctions(nullptr) {
    std::cout << "let's create life\n";
}

void nelo::init(std::list<std::function<void(float)>>* tickFunctions_) {
    _tickFunctions = tickFunctions_;
    std::cout << "I am Nelo\n";
    _tickFunctions->emplace_back([](float){ std::cout << "doing something...\n"; });
}

void nelo::destroy() {
    std::cout << "aaaaaannnd am ded =(\n";
}

nelo::~nelo() {
    std::cout << "truely destroyed\n";
}
