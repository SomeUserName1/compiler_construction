#include "NumberNode.h"

NumberNode::NumberNode(std::string name, int value, bool isConst) : DeclarationNode(std::move(name)), _value(value),
_isConst(isConst){}

const std::string NumberNode::getType() const {
  return this->_type;
}
const int NumberNode::getValue() const {
  return this->_value;
}
void NumberNode::setValue(int new_val) {
  if (this->_isConst)
    throw "You are trying to change a const value, namely" + this->getName();
  this->_value = new_val;
}

