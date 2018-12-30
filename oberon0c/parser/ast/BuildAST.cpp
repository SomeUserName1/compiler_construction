#include <vector>
#include "BuildAST.h"


BuildAST::BuildAST(std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<Node> parseTree)
{
	currentTable_ = symbolTable;
	parseTree_ = parseTree;
}

void BuildAST::build()
{
	std::vector<const Node*> children = parseTree_->getChildren();
	declarations(children.at(1));
	statementSequence(children.at(2));
}

void BuildAST::declarations(const Node * declarationsNode)
{
	std::vector<const Node*> children = declarationsNode->getChildren();

	for (auto declaration : children) {
		if (declaration->getNodeType() == NodeType::procedure_declaration) {
			procedureDeclaration(declaration);
		}
	}
}

const ASTNode* BuildAST::statementSequence(const Node * statementSequenceNode)
{
	ASTNode* statementSequenceAST = new ASTNode(ASTNodeType::statement_sequence);

	std::vector<const Node*> children = statementSequenceNode->getChildren();

	for (auto statementNode : children) {
		statementSequenceAST->addChild(statement(statementNode));
	}

	return statementSequenceAST;
}

void BuildAST::procedureDeclaration(const Node * procedureDeclarationNode)
{
	std::vector<const Node*> children = procedureDeclarationNode->getChildren();

	const Node* identifierNode = children.at(0)->getChildren().at(0);
	std::string procName = identifierNode->getValue();
	Symbol* procedureSymbol = currentTable_->getSymbol(&procName);

	const Node* statementSequenceNode = children.at(1)->getChildren().at(1);
	const ASTNode* statementSequenceAST = statementSequence(statementSequenceNode);

	procedureSymbol->setAst(statementSequenceAST);
}

const ASTNode* BuildAST::statement(const Node * statementNode)
{
	std::vector<const Node*> children = statementNode->getChildren();
	const Node* child = children.at(0);
	
	switch (child->getNodeType()) {
	case NodeType::assignment:
		return assignment(child);
	case NodeType::procedure_call:
		return procedureCall(child);
	case NodeType::if_statement:
		return ifStatement(child);
	case NodeType::while_statement:
		return whileStatement(child);
	default:
		throw std::invalid_argument("You failed!");
	}
}

const ASTNode * BuildAST::assignment(const Node * assignmentNode)
{
	return nullptr;
}

const ASTNode * BuildAST::procedureCall(const Node * procedureCallNode)
{
	return nullptr;
}

const ASTNode * BuildAST::ifStatement(const Node * ifStatementNode)
{
	return nullptr;
}

const ASTNode * BuildAST::whileStatement(const Node * whileStatementNode)
{
	return nullptr;
}
