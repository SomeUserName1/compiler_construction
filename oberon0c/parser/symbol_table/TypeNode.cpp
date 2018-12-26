#include "TypeNode.h"
TypeNode::TypeNode(std::string name, std::shared_ptr<Node> type)
  : DeclarationNode(std::move(name), DeclarationType::TYPE) {
  this->addChild(type);
}

