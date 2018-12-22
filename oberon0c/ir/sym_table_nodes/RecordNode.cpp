#include "RecordNode.h"
#include <utility>

RecordNode::RecordNode(std::shared_ptr<ASTNode> parent,
					   std::vector<std::shared_ptr<ASTNode>> children,
					   std::shared_ptr<SymbolTable> sym_table, std::string
					   name, int no_fields)
							: DeclarationNode(
		std::move(parent), std::move(children), std::move(sym_table),
		std::move(name)), _no_fields(no_fields)
{}

const int RecordNode::getNoFields() {
	return this->_no_fields;
}

const std::shared_ptr<DeclarationNode> RecordNode::getField(
		const std::string &name) {
	std::vector<std::shared_ptr<AS
	return std::shared_ptr<DeclarationNode>();
}

const std::vector<std::shared_ptr<DeclarationNode>> RecordNode::getFields() {
	return std::vector<std::shared_ptr<DeclarationNode>>();
}


