#include <iostream>
#include <PlatyInterface>
#include <glm/gtx/rotate_vector.hpp>
#include "nelo.hh"

nelo g_nelo;
nelo::nelo() {
    std::cout << "let's create life\n";
}

void nelo::test(float deltaTime) {
    //std::cout << "Node ptr:" << g_nelo.hierarchy_ << '\n';
    //std::cout << "deltaTime:" << deltaTime << '\n';
    glm::mat4& t = g_nelo.hierarchy_->getLocalTransformRef();
    g_nelo.hierarchy_->setLocalTransform(glm::rotate(t, 5 * deltaTime, glm::vec3(0, 1, 0)));
}

void(*nelo::getTickFun())(float) {
    return &nelo::test;
    //return [](float deltaTime){ 
    //    glm::mat4& t = hierarchy_->getLocalTransformRef();
    //    hierarchy_->setLocalTransform(glm::rotate(t, 5 * deltaTime, glm::vec3(0, 1, 0)));
    //    std::cout << "doing something...\n";
    //};
    //return [](float){};
}

unsigned int nelo::getRemainingTickFunSize() {
    return 1;
}

void nelo::init(Node* n_) {
    hierarchy_ = n_;
    std::cout << "I am Nelo\n";
}

void nelo::destroy() {
    std::cout << "aaaaaannnd am ded =(\n";
}

nelo::~nelo() {
    std::cout << "truely destroyed\n";
}

void init(Node* hierarchy) { g_nelo.init(hierarchy); std::cout << "pointer to node:" << hierarchy << '\n'; }
void(*getTickFun())(float) { return g_nelo.getTickFun(); }
unsigned int getRemainingTickFunSize() { return g_nelo.getRemainingTickFunSize(); }
void destroy() { g_nelo.destroy(); }
