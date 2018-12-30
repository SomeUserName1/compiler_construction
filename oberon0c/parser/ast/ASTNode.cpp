#include "ASTNode.h"

void ASTNode::printTreeRec(std::ostream & stream, int depth) const
{
	for (int i = 0; i < depth; i++) {
		stream << "|-";
	}

	stream << nodeType_;

	if (nodeType_ == ASTNodeType::symbol) {
		stream << " " << *symbol_;
	}

	stream << std::endl;

	for (auto child : children_) {
		child->printTreeRec(stream, depth + 1);
	}
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

void ASTNode::printTree(std::ostream & stream) const
{
	printTreeRec(stream, 0);
}

std::ostream & operator<<(std::ostream & stream, const ASTNode & node)
{
	node.printTree(stream);
	return stream;
}

std::ostream& operator<<(std::ostream &stream, const ASTNodeType &type) {
	std::string result;
	switch (type) {
	case ASTNodeType::statement_sequence: result = "statement_sequence"; break;
	case ASTNodeType::statement: result = "statement"; break;
	case ASTNodeType::if_statement: result = "if_statement"; break;
	case ASTNodeType::while_statement: result = "while_statement"; break;
	case ASTNodeType::assignment: result = "assignment"; break;
	case ASTNodeType::procedure_call: result = "procedure_call"; break;
	case ASTNodeType::symbol: result = "symbol"; break;
	case ASTNodeType::plus: result = "+"; break;
	case ASTNodeType::minus: result = "-"; break;
	case ASTNodeType::div: result = "DIV"; break;
	case ASTNodeType::times: result = "*"; break;
	case ASTNodeType:: or : result = "|"; break;
	case ASTNodeType::and: result = "&"; break;
	case ASTNodeType::leq: result = "<="; break;
	case ASTNodeType::lt: result = "<"; break;
	case ASTNodeType::geq: result = ">="; break;
	case ASTNodeType::gt: result = ">"; break;
	case ASTNodeType::eq: result = "="; break;
	case ASTNodeType::neq: result = "#"; break;
	case ASTNodeType::mod: result = "MOD"; break;
	case ASTNodeType::not: result = "~"; break;
	}
	stream << result;
	return stream;
}