#include "Node.h"

const std::vector<std::shared_ptr<Node>> Node::getChildren() const {
  return this->_children;
}
void Node::addChildren(std::vector<std::shared_ptr<Node>> newChildren) {
  this->_children.insert(_children.end(), newChildren.begin(), newChildren.end());
}

void Node::addChild(std::shared_ptr<Node> newChild) {
  this->_children.push_back(newChild);
}
Node::~Node() = default;
