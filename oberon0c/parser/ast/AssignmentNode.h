/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ASSIGNMENTNODE_H
#define OBERON0C_ASSIGNMENTNODE_H

#include <Node.h>
#include <symbol_table/DeclarationNode.h>

class AssignmentNode : public Node {
public:
  AssignmentNode(std::shared_ptr<DeclarationNode> lhs, std::shared_ptr<DeclarationNode> rhs);
  void print(std::ostream & stream) const override;
};

#endif //OBERON0C_ASSIGNMENTNODE_H
