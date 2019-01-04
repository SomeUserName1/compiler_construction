/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ARITHMETICOPNODE_H
#define OBERON0C_ARITHMETICOPNODE_H

#include <Node.h>
#include <symbol_table/NumberNode.h>

class ArithmeticOpNode : public NumberNode {
private:

public:
  ArithmeticOpNode(std::string operator_, std::shared_ptr<NumberNode> operand_0, std::shared_ptr<NumberNode> operand_1);
  ArithmeticOpNode(std::string operator_);
};

#endif //OBERON0C_ARITHMETICOPNODE_H
