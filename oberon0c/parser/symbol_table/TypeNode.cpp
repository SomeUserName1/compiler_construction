#include <utility>

#include <utility>

#include "TypeNode.h"
TypeNode::TypeNode(std::string name, std::shared_ptr<DeclarationNode> aliased, DeclarationType decl_type, std::string aliased_type)
  : DeclarationNode(std::move(name), decl_type), _type(std::move(aliased_type)) {
  this->addChild(std::move(aliased));
}
std::shared_ptr<DeclarationNode> TypeNode::getAliased() {
  auto aliased = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*this->getChildren()[0]));
  return aliased;
}
const void TypeNode::setAliasedValue(std::shared_ptr<DeclarationNode> node) {
  // TODO
  auto aliased = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*this->getChildren()[0]));
  aliased->
  return;
}
std::string TypeNode::getAliasedString() {
  return this->_type;
}

