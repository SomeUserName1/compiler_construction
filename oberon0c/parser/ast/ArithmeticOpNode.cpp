#include "ArithmeticOpNode.h"
ArithmeticOpNode::ArithmeticOpNode(std::string operator_, std::shared_ptr<NumberNode> operand_0,
    std::shared_ptr<NumberNode> operand_1) : NumberNode(std::move(operator_), DeclarationType::VAR){
  this->addChild({std::move(operand_0), std::move(operand_1)});
}

ArithmeticOpNode::ArithmeticOpNode(std::string operator_) : NumberNode(std::move(operator_), DeclarationType::VAR){}