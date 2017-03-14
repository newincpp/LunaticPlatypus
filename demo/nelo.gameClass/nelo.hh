
class nelo {
    private:
	//std::list<std::function<void(float)>> _tickFunctions;
    public:
	nelo();
	void init();
	void(*getTickFun())(float);
	unsigned int getRemainingTickFunSize();
	void destroy();
	~nelo();
};

