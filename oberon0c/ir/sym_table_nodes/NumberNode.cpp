#include "NumberNode.h"

NumberNode::NumberNode(std::shared_ptr<ASTNode> parent,
                       std::vector<std::shared_ptr<ASTNode>> children,
                       std::shared_ptr<SymbolTable> &sym_table,
                       std::string &name,
                       int value,
                       bool isConst)
                       : DeclarationNode(std::move(parent), std::move(children), sym_table, name),
                          _type(INTEGER), _value(value), _isConst
                                 (isConst){}

const type NumberNode::getType() const {
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

