#include "TypeNode.h"
TypeNode::TypeNode(std::string name, std::shared_ptr<Node> aliased, DeclarationType type)
  : DeclarationNode(std::move(name), type) {
  this->addChild(aliased);
}

// TODO set value over aliased if masked

// TODO