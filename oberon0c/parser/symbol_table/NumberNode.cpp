#include <utility>

#include <utility>

#include "NumberNode.h"

NumberNode::NumberNode(std::string name, int value, DeclarationType declarationType)
  : DeclarationNode(std::move(name), declarationType, "NumberNode"), _value(value), _init(true) {}

NumberNode::NumberNode(std::string name, DeclarationType declarationType) : DeclarationNode(std::move(name),
    declarationType, "NumberNode"){}

NumberNode::NumberNode(std::string name, DeclarationType declarationType, std::string type) : DeclarationNode(std::move(name),
                                                                                            declarationType, std::move(type)){}

const int NumberNode::getValue() const {
  if (!this->_init) {
    std::cerr << this->getName() << " is not initialized" << std::endl;
  }
  return this->_value;
}

void NumberNode::setValue(int new_val) {
  if (this->getDeclType() == DeclarationType::CONST)
    throw std::logic_error("You are trying to change a const value, namely" + this->getName());
  this->_value = new_val;
}

void NumberNode::print(std::ostream & stream) const {
  if(this->_init) {
    stream << "|_" << this->getName() << ", " << this->getDeclType() << ", " << this->getType() << ", "
           << this->getValue();
  } else if (this->getDeclType() == DeclarationType::OP) {
    stream << "|_" << this->getName();
  } else {
    stream << "|_" << this->getName() << ", " << this->getDeclType() << ", " << this->getType();
  }
}

void NumberNode::printTreeRec(std::ostream & stream, int depth) const
{
  for (int i = 0; i < depth; i++) {
    stream << "  ";
  }

  this->print(stream);
  stream << std::endl;

  for (const auto &child : this->getChildren()) {
    dynamic_cast<NumberNode &>(*child).printTreeRec(stream, depth + 1);
  }
}

std::ostream & operator<<(std::ostream & stream, const NumberNode & node) {
  node.printTree(stream);

  return stream;
}
