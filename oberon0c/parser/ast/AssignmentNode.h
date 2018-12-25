/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include <Node.h>
#include <symbol_table/DeclarationNode.h>

class AssignmentNode : public Node {
private:
  const std::string _ASSIGNMENT_STRING = ":=";

public:
  AssignmentNode(std::shared_ptr<DeclarationNode> lhs, std::shared_ptr<DeclarationNode> rhs);
};

#endif //OBERON0C_ASSIGNMENTNODE_H
