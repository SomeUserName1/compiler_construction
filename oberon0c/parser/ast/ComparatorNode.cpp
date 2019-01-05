#include <utility>
#include <assert.h>

#include "ComparatorNode.h"

ComparatorNode::ComparatorNode(std::string operator_) : BooleanNode(std::move(operator_)) {
  assert(this->getName() == "=" || this->getName() == "!=" || this->getName() == "<"  || this->getName() == "<="
      || this->getName() == ">" || this->getName() == ">=");
}

void ComparatorNode::addChild(std::shared_ptr <Node> newChild) {
  if (!this->getChildren().empty()) {
    auto check = dynamic_cast<DeclarationNode &>(*newChild);
    auto old_check = dynamic_cast<DeclarationNode &>(*this->getChildren()[0]);
    if(!(check.getType() == old_check.getType()))
      throw std::logic_error("type mismatch in comparator");

  }
  Node::addChild(newChild);
}