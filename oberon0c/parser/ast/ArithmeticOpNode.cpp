#include "ArithmeticOpNode.h"
ArithmeticOpNode::ArithmeticOpNode(std::string operator_, std::shared_ptr<NumberNode> operand_0,
    std::shared_ptr<NumberNode> operand_1) : NumberNode(std::move(operator_), DeclarationType::OP){
  this->addChild(std::move(operand_0));
  this->addChild(std::move(operand_1));
  assert(this->getName() == "+" || this->getName() == "-" || this->getName() == "*" || this->getName() == "/" || this->getName() == "%");
}

ArithmeticOpNode::ArithmeticOpNode(std::string operator_) : NumberNode(std::move(operator_), DeclarationType::OP) {
  assert(this->getName() == "+" || this->getName() == "-" || this->getName() == "*" || this->getName() == "/" || this->getName() == "%");
}

void ArithmeticOpNode::addChild(std::shared_ptr <Node> newChild) {
  auto check = dynamic_cast<DeclarationNode &>(*newChild);
  assert(check.getType() == "NumberNode");
  Node::addChild(newChild);
}