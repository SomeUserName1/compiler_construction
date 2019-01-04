#include "TypeNode.h"

TypeNode::TypeNode(std::string name, std::shared_ptr<DeclarationNode> aliased, DeclarationType decl_type)
  : DeclarationNode(std::move(name), decl_type, "TypeNode") {
  this->addChild(std::move(aliased));
}
std::shared_ptr<DeclarationNode> TypeNode::getAliased() {
  auto aliased = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*this->getChildren()[0]));
  return aliased;
}
void TypeNode::setValue(std::shared_ptr<DeclarationNode> node) {
  auto type = dynamic_cast<DeclarationNode&>(*this->getChildren()[0]);
  if (node->getType() == type.getType()) {
    this->setChild(0, node);
  } else {
    throw "Nope, invalid types";
  }
}

