#include <iostream>
#ifdef __APPLE__
#include <vector>
#elif __linux__
#include <deque>
#endif
#include <list>
#include "glm/glm.hpp"

class Node {
    private:
	std::string _name;
	glm::mat4 _lTransform;
	glm::mat4* _wTransform;
#ifdef __APPLE__
	std::vector<Node> _childs;
#elif __linux__
	std::deque<Node> _childs;
#endif
    public:
	Node(const std::string& n) : _name(n), _lTransform(1.0f), _wTransform(nullptr) {}
	void push(std::string n);
	Node& operator[](unsigned long i);
	void setLocalTransform(glm::mat4&& t_);
	void read(); 
	void readFromMe();
	void updateFromMe(glm::mat4* worldTransformParent_);
	void updateFromMe(Node&);
};

class Graph {
    public:
	Node root;
	std::list<std::pair<int, Node*>> dirty;
	Graph() : root("root") {}
	void update();
	void import();
};
