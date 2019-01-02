#include <utility>

#include "Node.h"

const std::vector<std::shared_ptr<Node>> Node::getChildren() const {
  return this->_children;
}
void Node::addChild(std::initializer_list<std::shared_ptr<Node>> newChild) {
  this->_children.insert(this->_children.end(), newChild);
}
void Node::addChild(std::shared_ptr<Node> newChild) {
  this->_children.push_back(newChild);
}

Node::Node(std::vector<std::shared_ptr<Node>> children) : _children(std::move(children)) {}
Node::Node() = default;
Node::~Node() = default;
