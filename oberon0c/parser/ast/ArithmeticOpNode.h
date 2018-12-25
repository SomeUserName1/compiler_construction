/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ARITHMETICOPNODE_H
#define OBERON0C_ARITHMETICOPNODE_H

#include <Node.h>
#include <symbol_table/NumberNode.h>
static char ADD = '+';
static char SUB = '-';
static char MULT = '*';
static char DIV = '/';
static char MOD = '%';

class ArithmeticOpNode : public Node {
private:
  char _operator;
  std::shared_ptr<NumberNode> _op_0;
  std::shared_ptr<NumberNode> _op_1;

public:
  ArithmeticOpNode(char operator_, std::shared_ptr<NumberNode> operand_0, std::shared_ptr<NumberNode> operand_1);
};

#endif //OBERON0C_ARITHMETICOPNODE_H
