#include <assert.h>
#include "ArrayNode.h"
#include "NumberNode.h"
#include "RecordNode.h"
#include "TypeNode.h"

ArrayNode::ArrayNode(std::string name, DeclarationType decl_type, int size, std::shared_ptr<DeclarationNode> type)
  : DeclarationNode(std::move(name), decl_type, "ArrayNode"), _size(size), _array_type(type) {
  std::string type_name = type->getType();
  if (type_name == "NumberNode") {
    for (int i = 0; i < _size; ++i) {
      this->addChild(std::make_shared<NumberNode>(name + to_string(i), 0, this->getDeclType()));
    }
  } else if (type_name == "RecordNode") {
    for (int i = 0; i < _size; ++i) {
      auto record_type = dynamic_cast<RecordNode &>(*type);
      auto record_scope = std::make_shared<SymbolScopeNode>(dynamic_cast<SymbolScopeNode &>(*record_type.getChildren()[0]));
      this->addChild(std::make_shared<RecordNode>(name + to_string(i), this->getDeclType(), record_scope));
    }
  } else if (type_name == "ArrayNode") {
    for (int i = 0; i < _size; ++i) {
      auto array_type = dynamic_cast<ArrayNode &>(*type);
      this->addChild(std::make_shared<ArrayNode>(name + to_string(i), this->getDeclType(), array_type.getSize(),
          array_type.getArrayType()));
    }
  } else if (type_name == "TypeNode") {
    for (int i = 0; i < _size; ++i) {
      auto type_type = dynamic_cast<TypeNode &>(*type);
      this->addChild(std::make_shared<TypeNode>(name + to_string(i), type_type.getAliased(), this->getDeclType()));
    }
  }
}

const int ArrayNode::getSize() {
  return this->_size;
}

const std::shared_ptr<DeclarationNode> ArrayNode::getArrayType() {
  return _array_type;
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
  if (position < 0 || position > this->getChildren().size()-1)
    throw std::logic_error("arrays out of bounds" + this->getName() + " actual size " + to_string(this->getChildren().size()) +
     " but tried to access " + to_string(position +1) + " element");
  auto *result = dynamic_cast<DeclarationNode *>(&*this->getChildren()[position]);
  return std::shared_ptr<DeclarationNode>(result);
}

void ArrayNode::setValue(int position, std::shared_ptr<DeclarationNode> node) {
  if (node->getType() == this->_array_type->getType()) {
    this->setChild(position, node);
  } else {
    throw std::logic_error("Nope, invalid types");
  }
}
