#pragma once
#include <string>
#include <list>
#include <map>
#include <functional>
#include "DynamicGameClass.hh"

class StaticGameClassGenerator {
    private:
        static std::map<std::string, GameClass*(*)(Node&)> _ctorMap;
    public:
        static void addImpl(std::string, GameClass*(*)(Node&));
        static GameClass* gen(const std::string&, Node& n_);
        template<typename T>
        static GameClass* createNew(Node& node_) { return new T(node_); }
};

#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define STATIC_CLASS_ADD(type) StaticGameClassGenerator::addImpl(STRINGIZE2(type), &StaticGameClassGenerator::createNew<type>)
