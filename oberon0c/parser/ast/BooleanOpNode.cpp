#include <utility>

#include "BooleanOpNode.h"
BooleanOpNode::BooleanOpNode(std::string operator_) : BooleanNode(operator_) {
  assert(this->getName() == "&" || this->getName() == "|");
  }
void BooleanOpNode::addChild(std::shared_ptr<Node> newChild) {
  auto check = dynamic_cast<DeclarationNode &>(*newChild);
  assert(check.getType() == "BooleanNode");
  Node::addChild(newChild);
}
