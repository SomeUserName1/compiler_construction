#include <utility>
#include "ASTNode.h"

ASTNode::ASTNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children) :
  _parent(std::move(parent)), _children(std::move(children)) {}

const std::shared_ptr<ASTNode> ASTNode::getParent() {
  return std::shared_ptr<ASTNode>();
}
const std::vector<std::shared_ptr<ASTNode>> ASTNode::getChildren() {
  return std::vector<std::shared_ptr<ASTNode>>();
}
void ASTNode::addChildren(std::vector<std::shared_ptr<ASTNode>> newChildren) {
  this->_children.insert(_children.end(), newChildren.begin(), newChildren.end());
}

