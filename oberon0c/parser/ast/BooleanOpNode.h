/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_BOOLEANOPNODE_H
#define OBERON0C_BOOLEANOPNODE_H

#include <Node.h>
#include <symbol_table/BooleanNode.h>
static char AND = '&';
static char OR = '|';

class BooleanOpNode : public Node {
private:
  char _operator;
  std::shared_ptr<BooleanNode> _op_0;
  std::shared_ptr<BooleanNode> _op_1;

public:
  BooleanOpNode(char operator_, std::shared_ptr<BooleanNode> operand_0, std::shared_ptr<BooleanNode> operand_1);
};

#endif //OBERON0C_BOOLEANOPNODE_H
