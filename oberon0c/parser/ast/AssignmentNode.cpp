#include "AssignmentNode.h"
AssignmentNode::AssignmentNode(std::shared_ptr<DeclarationNode> lhs, std::shared_ptr<DeclarationNode> rhs)
  : Node({lhs,rhs}){
}
