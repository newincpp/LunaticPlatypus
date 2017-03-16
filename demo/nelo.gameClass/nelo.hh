class nelo {
    public:
	nelo();
	void init();
	void(*getTickFun())(float);
	unsigned int getRemainingTickFunSize();
	void destroy();
	~nelo();
};
