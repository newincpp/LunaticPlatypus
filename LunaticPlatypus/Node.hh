#pragma once
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
	std::list<Node> _childs;
    public:
	Node(const std::string& n);
	Node& push(std::string&& n);
	Node& push();
	//Node& operator[](unsigned long i);
	void setLocalTransform(glm::mat4&& t_);
	void linkWorldTransform(glm::mat4*);
	glm::mat4& getLocalTransformRef();
	void read(Node&); 
	void readFromMe(Node&);
	void updateFromMe(glm::mat4*, std::list<glm::mat4*>&, bool = false);
	void updateFromMe(Node&, std::list<glm::mat4*>&, bool = false);
};

class Graph {
    public:
        bool _valid;
	Node root;
	std::list<std::pair<int, Node*>> dirty;
	std::list<glm::mat4*> toBeDeleted;
	Graph();
	void update(bool=false);
	void remove(glm::mat4*);
	void import();
};
