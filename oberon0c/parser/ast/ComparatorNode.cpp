#include <utility>

#include "ComparatorNode.h"
ComparatorNode::ComparatorNode(std::string operator_,
                               std::shared_ptr<DeclarationNode> operand_0,
                               std::shared_ptr<DeclarationNode> operand_1) : _op_0(std::move(operand_0)),
                               _op_1(std::move(operand_1)){
  if (operator_ == EQUALS || operator_ == NOT_EQUALS || operator_ == LESS_THAN  || operator_ == LESS_EQUAL
    || operator_ == GREATER_THAN || operator_ == GREATER_EQUAL) {
      this->_operator = operator_;
  } else {
    throw "Wrong comparator string" + operator_;
  }
}
