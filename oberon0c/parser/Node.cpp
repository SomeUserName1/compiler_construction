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
void Node::setChild(int position, std::shared_ptr<Node> new_child) {
  this->_children[position] = new_child;
}
Node::Node(std::vector<std::shared_ptr<Node>> children) : _children(std::move(children)) {}
Node::Node() = default;
Node::~Node() = default;

void Node::print(std::ostream & stream) const {
  stream << "|_ Node (default)";
}

void Node::printTree(std::ostream & stream) const {
  printTreeRec(stream, 0);
}

void Node::printTreeRec(std::ostream & stream, int depth) const
{
  for (int i = 0; i < depth; i++) {
    stream << "  ";
  }

  this->print(stream);
  stream << std::endl;

  for (const auto &child : this->getChildren()) {
    child->printTreeRec(stream, depth + 1);
  }
}

std::ostream & operator<<(std::ostream & stream, const Node & node) {
  node.printTree(stream);

  return stream;
}