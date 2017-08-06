#include <string>
#include <list>
#include <map>
#include <functional>

class StaticGameClass {
    private:
        static std::map<std::string, std::function<void(void)>> _ctorMap;
        static std::list<void*> _allocated;
    public:
        static void freeAll();
        static void addImpl(std::string&, std::function<void(void)>&);
        static bool gen(const std::string&);
        template<typename T>
        static void* createNew() { return new T(); }
};
#define ADD(name, type) addImpl(name, std::bind(StaticGameClass::createNew<type>))
