#include "BooleanNode.h"

BooleanNode::BooleanNode(std::string name, int value) : NumberNode(std::move(name), DeclarationType::VAR, "BooleanNode") {
  assert( value == 0 || value == 1);
    this->setValue(value);
}
BooleanNode::BooleanNode(std::string name) : NumberNode(std::move(name), DeclarationType::VAR, "BooleanNode") {

}

void BooleanNode::setValue(int value) {
  if (!(value == 0 || value == 1))
    throw std::logic_error("Boolean must be of value 0 or 1");
  this->setValue(value);
}
