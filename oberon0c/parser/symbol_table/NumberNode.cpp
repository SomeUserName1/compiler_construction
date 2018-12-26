#include "NumberNode.h"

NumberNode::NumberNode(std::string name, int value, DeclarationType declarationType)
  : DeclarationNode(std::move(name), declarationType), _value(value) {}

const std::string NumberNode::getType() const {
  return this->_type;
}
const int NumberNode::getValue() const {
  return this->_value;
}
void NumberNode::setValue(int new_val) {
  if (this->getDeclType() == DeclarationType::CONST)
    throw "You are trying to change a const value, namely" + this->getName();
  this->_value = new_val;
}

