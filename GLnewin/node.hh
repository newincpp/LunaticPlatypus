#include <iostream>
#include <deque>
#include <list>
#include "glm/glm.hpp"

class Node {
    private:
	std::string _name;
	glm::mat4 _lTransform;
	glm::mat4* _wTransform;
	std::deque<Node> _childs;
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
