#include <utility>

#include "NumberNode.h"

NumberNode::NumberNode(std::string name, int value, DeclarationType declarationType)
  : DeclarationNode(std::move(name), declarationType, "NumberNode"), _value(value) {}

NumberNode::NumberNode(std::string name, DeclarationType declarationType) : DeclarationNode(std::move(name),
    declarationType, "NumberNode"){}

const int NumberNode::getValue() const {
  return this->_value;
}

void NumberNode::setValue(int new_val) {
  if (this->getDeclType() == DeclarationType::CONST)
    throw "You are trying to change a const value, namely" + this->getName();
  this->_value = new_val;
}

