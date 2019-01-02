#include <utility>

#include "ArrayNode.h"

ArrayNode::ArrayNode(std::string name, DeclarationType decl_type, std::vector<std::shared_ptr<Node>> elements, int size, std::string array_type)
  : DeclarationNode(std::move(name), decl_type, std::move(elements)), _size(size),
  _type(std::move(array_type)) {
  // TODO handle children here? how to handle type? switch case over primitive (INTEGER) then look up for record and typedef?
}

// TODO set value

const int ArrayNode::getSize() {
  return this->_size;
}

const std::string ArrayNode::getType() {
  return this->_type;
}

const std::vector<std::shared_ptr<DeclarationNode>> ArrayNode::getValues() {
  std::vector<std::shared_ptr<DeclarationNode>> res;
  res.reserve(this->getChildren().size());

  for (const auto &node : this->getChildren()) {
    res.push_back(std::shared_ptr<DeclarationNode>(&dynamic_cast<DeclarationNode &>(*node)));
  }
  return res;
}

const std::shared_ptr<DeclarationNode> ArrayNode::getValue(int position) {
  auto *result = dynamic_cast<DeclarationNode *>(&*this->getChildren()[position]);
  return std::shared_ptr<DeclarationNode>(result);
}
