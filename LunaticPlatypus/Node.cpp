#include "Node.hh"
#include "glm/gtx/string_cast.hpp"

#include <algorithm>

Node::Node(const std::string& n) : _name(n), _lTransform(1.0f), _wTransform(nullptr) {
}
Node& Node::push(std::string&& n_) {
    _childs.emplace_back(std::move(n_));
    return _childs.back();
}
Node& Node::push() {
    _childs.emplace_back("");
    return _childs.back();
}
Node& Node::operator[](unsigned long i) {
    return _childs[i];
}
void Node::setLocalTransform(glm::mat4&& t_) {
    _lTransform = t_;
}
glm::mat4& Node::getLocalTransformRef() {
    return _lTransform;
}
void Node::linkWorldTransform(glm::mat4* link_) {
    _wTransform = link_;
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

void Node::updateFromMe(Node& p, std::list<glm::mat4*>& del, bool localMode) {
    updateFromMe(p._wTransform, del, localMode);
}

void Node::updateFromMe(glm::mat4* worldTransformParent_, std::list<glm::mat4*>& del, bool localMode) {
    std::list<glm::mat4*>::iterator it = std::find(del.begin(), del.end(), _wTransform);
    if (it != del.end()) {
	_wTransform = nullptr;
	del.erase(it);
    }
    if (!worldTransformParent_) {
	std::cout << "fk u\n";
	return;
    } 
    
    glm::mat4* worldRef;
    glm::mat4 tmpw;
    if (_wTransform) {
	if (localMode) {
	    _lTransform = *_wTransform * glm::inverse(*worldTransformParent_);
	} else {
	    *_wTransform = _lTransform * *worldTransformParent_;
	}
	worldRef = _wTransform;
    } else {
	tmpw = _lTransform * *worldTransformParent_;
	worldRef = &tmpw;
    }
    for (Node& n : _childs) {
	n.updateFromMe(worldRef, del, localMode);
    }
}

Graph::Graph() : root("root") {
    root.linkWorldTransform(new glm::mat4(1.0f));
    dirty.push_back(std::make_pair(0, &root));
}

void Graph::update(bool localMode) {
    for (std::pair<int,Node*>& d : dirty) {
	d.second->updateFromMe(root, toBeDeleted, localMode);
    }
}

void Graph::remove(glm::mat4* del) {
    toBeDeleted.emplace_back(del);
}

void Graph::import() {
}