#include <vector>
#include "BuildAST.h"
#include <list>
#include <algorithm>

BuildAST::BuildAST(std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<Node> parseTree)
{
	currentTable_ = symbolTable;
	parseTree_ = parseTree;
	anonymousCounter_ = 0;
}

void BuildAST::build()
{
	std::vector<const Node*> children = parseTree_->getChildren();
	declarations(children.at(1));
	const ASTNode* node = statementSequence(children.at(2));

	std::string moduleIdent = children.at(0)->getValue();
	Symbol* moduleSymbol = currentTable_->getSymbol(&moduleIdent);

	moduleSymbol->setAst(node);
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
		default:
			throw std::invalid_argument("You failed!");
		}

		ASTNode* node = new ASTNode(astType);
		node->addChild(simpleExpression(se));
		nodes.push_back(node);
	}

	for (size_t i = 0; i < nodes.size() - 1; i++) {
		nodes.at(i)->addChild(nodes.at(i + 1));
	}
	nodes.back()->addChild(simpleExpression(simpleExpressions.back()));

	return nodes.front();
}

const ASTNode * BuildAST::simpleExpression(const Node * simpleExpressionNode)
{
	std::vector<const Node*> children = simpleExpressionNode->getChildren();

	if (children.size() == 1) {
		return term(children.at(0));
	}

	std::list<const Node*> operators;
	std::list<const Node*> terms;


	for (const Node* child : children) {
		if (child->getNodeType() == NodeType::term) {
			terms.push_back(child);
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	std::vector<ASTNode*> nodes;
	while (terms.size() > 1) {
		const Node* op = operators.front();
		const Node* se = terms.front();
		operators.pop_front();
		terms.pop_front();

		ASTNodeType astType;
		switch (op->getNodeType()) {
		case NodeType::plus:
			astType = ASTNodeType::plus;
			break;
		case NodeType::minus:
			astType = ASTNodeType::minus;
			break;
		case NodeType::_or:
			astType = ASTNodeType::_or;
			break;
		default:
			throw std::invalid_argument("You failed!");
		}

		ASTNode* node = new ASTNode(astType);
		node->addChild(term(se));
		nodes.push_back(node);
	}

	for (size_t i = 0; i < nodes.size() - 1; i++) {
		nodes.at(i)->addChild(nodes.at(i + 1));
	}
	nodes.back()->addChild(term(terms.back()));

	return nodes.front();
}

const ASTNode * BuildAST::term(const Node * termNode)
{
	std::vector<const Node*> children = termNode->getChildren();



	if (children.size() == 1) {
		return factor(children.at(0));
	}

	std::list<const Node*> operators;
	std::list<const Node*> factors;


	for (const Node* child : children) {
		if (child->getNodeType() == NodeType::factor) {
			factors.push_back(child);
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	std::vector<ASTNode*> nodes;
	while (factors.size() > 1) {
		const Node* op = operators.front();
		const Node* se = factors.front();
		operators.pop_front();
		factors.pop_front();

		ASTNodeType astType;
		switch (op->getNodeType()) {
		case NodeType::times:
			astType = ASTNodeType::times;
			break;
		case NodeType::div:
			astType = ASTNodeType::div;
			break;
		case NodeType:: mod:
			astType = ASTNodeType:: mod ;
			break;
		case NodeType::_and:
			astType = ASTNodeType::_and;
			break;
		default:
			throw std::invalid_argument("You failed!");
		}

		ASTNode* node = new ASTNode(astType);
		node->addChild(factor(se));
		nodes.push_back(node);
	}

	for (size_t i = 0; i < nodes.size() - 1; i++) {
		nodes.at(i)->addChild(nodes.at(i + 1));
	}
	nodes.back()->addChild(factor(factors.back()));

	return nodes.front();
}

const ASTNode * BuildAST::factor(const Node * factorNode)
{
	std::vector<const Node*> children = factorNode->getChildren();
	switch (children.at(0)->getNodeType()) {
	case NodeType::identifier: {
		return identifier(&children);
	}
	case NodeType::number: {
		return number(children.at(0));
	}
	case NodeType::expression:
		return expression(children.at(0));
	case NodeType::factor: {
		return _not(children.at(0));
	}
	default:
		throw std::invalid_argument("You failed!");
	}
}

const ASTNode * BuildAST::identifier(std::vector<const Node*>* children)
{
	std::shared_ptr<SymbolTable> oldTable = currentTable_;
	const Node* ident = lastSelectorVariable(children, &currentTable_);
	std::string identName = ident->getValue();
	Symbol* identSymbol = currentTable_->getSymbol(&identName);

	ASTNode* node = new ASTNode(ASTNodeType::symbol, identSymbol);
	currentTable_ = oldTable;
	return node;
}

const ASTNode * BuildAST::number(const Node * numberNode)
{
	Symbol* anonymousSymbol = createAnonymousSymbol(numberNode);
	ASTNode* node = new ASTNode(ASTNodeType::symbol, anonymousSymbol);
	return node;
}

const ASTNode * BuildAST::_not(const Node * factorNode)
{
	ASTNode* node = new ASTNode(ASTNodeType::_not);
	node->addChild(factor(factorNode));
	return node;
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

Symbol * BuildAST::createAnonymousSymbol(const Node* numberNode)
{
	std::string constIdent = std::string("CONSTANT");
	Symbol* constant = currentTable_->getSymbol(&constIdent);
	std::vector<Symbol*> types;
	types.push_back(constant);

	std::stringstream ss;
	ss << "const" << std::to_string(anonymousCounter_++);
	std::string str = ss.str();
	Symbol newSymbol(str, types, SymbolType::constant, true, currentTable_);
	newSymbol.setValue(stoi(numberNode->getValue()));

	if (currentTable_->insert(newSymbol)) {
		throw std::invalid_argument("Symbol does already exist.");
	}

	return currentTable_->getSymbol(&str);
}
