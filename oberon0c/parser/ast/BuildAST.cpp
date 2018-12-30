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
		auto statemnt = statement(statementNode);
		if (statemnt != nullptr) {
			statementSequenceAST->addChild(statemnt);
		}
	}

	return statementSequenceAST;
}

void BuildAST::procedureDeclaration(const Node * procedureDeclarationNode)
{
	std::vector<const Node*> children = procedureDeclarationNode->getChildren();

	const Node* identifierNode = children.at(0)->getChildren().at(0);
	std::string procName = identifierNode->getValue();
	Symbol* procedureSymbol = currentTable_->getSymbol(&procName);

	currentTable_ = currentTable_->getChild(procName);
	const Node* statementSequenceNode = children.at(1)->getChildren().at(1);
	const ASTNode* statementSequenceAST = statementSequence(statementSequenceNode);
	currentTable_ = currentTable_->getParent();

	procedureSymbol->setAst(statementSequenceAST);
}

const ASTNode* BuildAST::statement(const Node * statementNode)
{

	std::vector<const Node*> children = statementNode->getChildren();
	if (children.size() == 0) {
		// empty statement.
		return nullptr;
	}
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
	ASTNode* node = new ASTNode(ASTNodeType::assignment);
	std::vector<const Node*> children = assignmentNode->getChildren();

	// Processing identifier
	std::shared_ptr<SymbolTable> oldTable = currentTable_;
	const Node* identifierNode = lastSelectorVariable(&children, &currentTable_);
	std::string identifier = identifierNode->getValue();
	Symbol* identifierSymbol = currentTable_->getSymbol(&identifier);
	ASTNode* identAST = new ASTNode(ASTNodeType::symbol, identifierSymbol);

	currentTable_ = oldTable;


	// Processing the expression
	const Node* expressionNode = children.back();
	const ASTNode* expressionAST = expression(expressionNode);

	
	node->addChild(identAST);
	node->addChild(expressionAST);

	return node;
}

const ASTNode * BuildAST::procedureCall(const Node * procedureCallNode)
{
	std::vector<const Node*> children = procedureCallNode->getChildren();
	const Node* identifierNode = children.at(0);
	std::string identifier = identifierNode->getValue();
	Symbol* procedureSymbol = currentTable_->getSymbol(&identifier);

	ASTNode* procedureCallAST = new ASTNode(ASTNodeType::procedure_call, procedureSymbol);
	
	return procedureCallAST;
}

const ASTNode * BuildAST::ifStatement(const Node * ifStatementNode)
{
	ASTNode* node = new ASTNode(ASTNodeType::if_statement);

	std::vector<const Node*> children = ifStatementNode->getChildren();
	for (size_t i = 0; i < children.size(); i++) {
		const Node* child = children.at(i);

		if (child->getNodeType() == NodeType::expression) {
			node->addChild(expression(child));
		}
		else {
			node->addChild(statementSequence(child));
		}
	}

	return node;
}

const ASTNode * BuildAST::whileStatement(const Node * whileStatementNode)
{
	std::vector<const Node*> children = whileStatementNode->getChildren();

	// Processing the expression
	const Node* expressionNode = children.at(0);
	const ASTNode* expressionAST = expression(expressionNode);


	// Processing the statementSequence
	const Node* statementSequenceNode = children.at(1);
	const ASTNode* statementSequenceAST = statementSequence(statementSequenceNode);

	// Add children to whileStatement AST node and return.
	ASTNode* whileStatementAST = new ASTNode(ASTNodeType::while_statement);
	whileStatementAST->addChild(expressionAST);
	whileStatementAST->addChild(statementSequenceAST);

	return whileStatementAST;
}

const ASTNode * BuildAST::expression(const Node * expressionNode)
{
	std::vector<const Node*> children = expressionNode->getChildren();

	if (children.size() == 1) {
		return simpleExpression(children.at(0));
	}

	std::list<const Node*> operators;
	std::list<const Node*> simpleExpressions;


	for (const Node* child : children) {
		if (child->getNodeType() == NodeType::simple_expression) {
			simpleExpressions.push_back(child);
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	std::vector<ASTNode*> nodes;
	while (simpleExpressions.size() > 1) {
		const Node* op = operators.front();
		const Node* se = simpleExpressions.front();
		operators.pop_front();
		simpleExpressions.pop_front();

		ASTNodeType astType;
		switch (op->getNodeType()) {
		case NodeType::eq:
			astType = ASTNodeType::eq;
			break;
		case NodeType::neq:
			astType = ASTNodeType::neq;
			break;
		case NodeType::lt:
			astType = ASTNodeType::lt;
			break;
		case NodeType::leq:
			astType = ASTNodeType::leq;
			break;
		case NodeType::gt:
			astType = ASTNodeType::gt;
			break;
		case NodeType::geq:
			astType = ASTNodeType::geq;
			break;
		}

		ASTNode* node = new ASTNode(astType);
		node->addChild(simpleExpression(se));
		nodes.push_back(node);
	}

	for (size_t i = 0; i < nodes.size() - 1; i++) {
		nodes.at(i)->addChild(nodes.at(i + 1));
	}

	return nodes.front();
}

const ASTNode * BuildAST::simpleExpression(const Node * simpleExpressionNode)
{
	return nullptr;
}


const Node * BuildAST::lastSelectorVariable(std::vector<const Node*>* children, std::shared_ptr<SymbolTable>* table)
{
	const Node* lastSelector = children->at(0);
	std::string lastSelName = lastSelector->getValue();
	auto possibleChildSymbolTable = (*table)->getChild(lastSelName);
	if (possibleChildSymbolTable != nullptr) {
		*table = possibleChildSymbolTable;
	}

	for (auto possibleSelector : *children) {
		if (possibleSelector->getNodeType() == NodeType::selector
			&& possibleSelector->getChildren().at(0)->getNodeType() == NodeType::identifier) {
			lastSelector = possibleSelector->getChildren().at(0);

			lastSelName = lastSelector->getValue();
			possibleChildSymbolTable = (*table)->getChild(lastSelName);
			if (possibleChildSymbolTable != nullptr) {
				*table = possibleChildSymbolTable;
			}
		}
	}

	return lastSelector;
}