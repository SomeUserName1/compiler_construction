#include "DeclarationNode.h"

DeclarationNode::DeclarationNode(std::string name) : _name(std::move(name)) {}

const std::string DeclarationNode::getName() const {
  return this->_name;
}
