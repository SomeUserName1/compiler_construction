#include "ArithmeticOpNode.h"
ArithmeticOpNode::ArithmeticOpNode(char operator_, std::shared_ptr<NumberNode> operand_0,
    std::shared_ptr<NumberNode> operand_1) : _op_0(std::move(operand_0)),
    _op_1(std::move(operand_1)) {
  switch (operator_) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '%': {
    this->_operator = operator_;
    break;
  }
  default:
    throw "Wrong operator char" + std::string() + operator_;
  }
}
