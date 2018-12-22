#include "ArrayNode.h"

ArrayNode::ArrayNode(std::shared_ptr<ASTNode> parent,
                     std::vector<std::shared_ptr<ASTNode>> children,
                     std::shared_ptr<SymbolTable> sym_table,
                     std::string name,
                     int size,
                     std::string type)
                     : DeclarationNode(std::move(parent), std::move(children), std::move(sym_table), std::move(name)),
                          _size(size), _type(std::move(type)) {}

const int ArrayNode::getSize() {
  return this->_size;
}

const std::string ArrayNode::getType() {
  return this->_type;
}

const std::vector<std::shared_ptr<NumberNode>> ArrayNode::getValues() {
  std::vector<std::shared_ptr<ASTNode>> values = this->getChildren();
  switch(reinterpret_cast<NumberNode &>(*values[0]).getType()) {
  case "int":
    std::vector<std::shared_ptr<NumberNode>> number_values(values.begin(), values.end());
    return number_values;
  } else {
    throw "No types but Integer supported so far";
  }
}

const std::shared_ptr<NumberNode> ArrayNode::getValue(int position) {
  std::shared_ptr<ASTNode> value = this->getChildren()[position];
  NumberNode result = reinterpret_cast<NumberNode &>(*value);

  return std::shared_ptr<NumberNode>(&result);
}
