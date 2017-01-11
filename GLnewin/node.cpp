#include "node.hh"

void Node::push(std::string n) {
    _childs.emplace_back(n);
}
Node& Node::operator[](unsigned long i) {
    return _childs[i];
}
void Node::setLocalTransform(glm::mat4&& t_) {
    _lTransform = t_;
}
void Node::read() {
    //std::cout << "Node:" << _name << "\t\ttransform:" << _transform << "\t child size:" << _childs.size() << '\n';
}
void Node::readFromMe() {
    read();
    for (Node& n : _childs) {
	n.readFromMe();
    }
}

void Node::updateFromMe(Node& p) {
    updateFromMe(p._wTransform);
}

void Node::updateFromMe(glm::mat4* worldTransformParent_) {
    if (!_wTransform) {
	return;
    }
    *_wTransform = *worldTransformParent_ * _lTransform;
    for (Node& n : _childs) {
	n.updateFromMe(_wTransform);
    }
}

void Graph::update() {
    for (std::pair<int,Node*>& d : dirty) {
	d.second->updateFromMe(root);
    }
}

void Graph::import() {
}
