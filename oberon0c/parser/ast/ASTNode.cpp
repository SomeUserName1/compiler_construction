#include "ASTNode.h"

void ASTNode::printTreeRec(std::ostream & stream, int depth) const
{
}

ASTNode::ASTNode(ASTNodeType nodeType)
{
	nodeType_ = nodeType;
}

ASTNode::ASTNode(ASTNodeType nodeType, Symbol * symbol)
{
	nodeType_ = nodeType;
	symbol_ = symbol;
}

const ASTNodeType ASTNode::getNodeType() const
{
	return nodeType_;
}

std::vector<const ASTNode*> ASTNode::getChildren() const
{
	return children_;
}

void ASTNode::addChild(const ASTNode * node)
{
	children_.push_back(node);
}

void ASTNode::addChilds(std::shared_ptr<std::vector<const ASTNode*>> nodes)
{
	for (const ASTNode* node : *nodes) {
		addChild(node);
	}
}

void ASTNode::print(std::ostream & stream) const
{
}

void ASTNode::printTree(std::ostream & stream) const
{
}

std::ostream & operator<<(std::ostream & stream, const ASTNode & node)
{
	return stream;
}
