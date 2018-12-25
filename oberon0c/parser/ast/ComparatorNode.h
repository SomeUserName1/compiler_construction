/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_COMPARATORNODE_H
#define OBERON0C_COMPARATORNODE_H

#include <Node.h>
#include <symbol_table/DeclarationNode.h>
static std::string EQUALS = "=";
static std::string NOT_EQUALS = "!=";
static std::string LESS_THAN = "<";
static std::string LESS_EQUAL = "<=";
static std::string GREATER_THAN = ">";
static std::string GREATER_EQUAL = ">=";

class ComparatorNode : public Node {
private:
  std::string _operator;
  std::shared_ptr<DeclarationNode> _op_0;
  std::shared_ptr<DeclarationNode> _op_1;

public:
  ComparatorNode(std::string operator_, std::shared_ptr<DeclarationNode> operand_0, std::shared_ptr<DeclarationNode> operand_1);
};

#endif //OBERON0C_COMPARATORNODE_H
