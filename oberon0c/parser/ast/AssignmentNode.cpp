#include "AssignmentNode.h"
AssignmentNode::AssignmentNode(std::shared_ptr<DeclarationNode> lhs, std::shared_ptr<DeclarationNode> rhs) {
   this->addChild(lhs);
   this->addChild(rhs);
}
