#include "BooleanNode.h"

BooleanNode::BooleanNode(std::string name, int value) : DeclarationNode(std::move(name), DeclarationType::VAR, "BooleanNode") {
  this->setValue(value);
}

const int BooleanNode::getValue() const {
  return this->_value;
}
void BooleanNode::setValue(int new_val) {
  if (new_val < 0 || new_val > 1) {
    throw "Boolean expression evaluated to something smaller than 0 or bigger than 1" + std::to_string(new_val);
  } else {
    this->_value = static_cast<bool>(new_val);
  }
}
