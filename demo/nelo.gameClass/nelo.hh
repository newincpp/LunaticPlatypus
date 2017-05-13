class nelo {
    private:
	Node* hierarchy_;
    public:
	nelo();
	void init(Node*);
	static void test(float);
	void(*getTickFun())(float);
	unsigned int getRemainingTickFunSize();
	void destroy();
	~nelo();
};
