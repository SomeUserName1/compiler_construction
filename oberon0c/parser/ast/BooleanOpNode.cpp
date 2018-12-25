#include <utility>

#include "BooleanOpNode.h"
BooleanOpNode::BooleanOpNode(char operator_,
                             std::shared_ptr<BooleanNode> operand_0,
                             std::shared_ptr<BooleanNode> operand_1) : _op_0(std::move(operand_0)),
                             _op_1(std::move(operand_1)) {
  if (operator_ == '&' || operator_ == '|') {
    this->_operator = operator_;
  } else {
    throw "Wrong operator string" + std::string() + operator_;
  }
}
