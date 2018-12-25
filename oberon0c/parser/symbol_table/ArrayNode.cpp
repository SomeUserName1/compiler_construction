#include "ArrayNode.h"

ArrayNode::ArrayNode(std::string name, int size, std::string type) : DeclarationNode(std::move(name)), _size(size),
  _type(std::move(type)) {
  // TODO handle children here? how to handle type? switch case over primitive (INTEGER) then look up for record and typedef?
}

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
