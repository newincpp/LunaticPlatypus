#include <PlatyInterface>

class nelo : public PlatyInterface {
    private:
	std::list<std::function<void(float)>>* _tickFunctions;
    public:
	nelo();
	virtual void init(std::list<std::function<void(float)>>* tickFunctions_);
	virtual void destroy();
	virtual ~nelo();
};

#ifdef __cplusplus
extern "C" {
#endif
PlatyInterface* genClass() {
    return new nelo;
}
#ifdef __cplusplus
}
#endif

