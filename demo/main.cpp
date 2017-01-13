#include <PlatypusHeart.hh>
#include <iostream>

class MyGame : public IGame {
    public:
	virtual void init() {}
	virtual void update() {}
	virtual void deInit() {}
};

SetGameClass(MyGame)
