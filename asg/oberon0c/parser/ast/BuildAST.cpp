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
	const std::shared_ptr<ASTNode> node = statementSequence(children.at(2));

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

const std::shared_ptr<ASTNode> BuildAST::statementSequence(const Node * statementSequenceNode)
{
	auto statementSequenceAST = std::make_shared<ASTNode>(ASTNodeType::statement_sequence);

	std::vector<const Node*> children = statementSequenceNode->getChildren();

	for (auto statementNode : children) {
		auto statemnt = statement(statementNode);
		if (statemnt != nullptr) {
			statementSequenceAST->addChildBack(statemnt);
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
	const std::shared_ptr<ASTNode> statementSequenceAST = statementSequence(statementSequenceNode);
	currentTable_ = currentTable_->getParent();

	procedureSymbol->setAst(statementSequenceAST);
}

const std::shared_ptr<ASTNode> BuildAST::statement(const Node * statementNode)
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

const std::shared_ptr<ASTNode> BuildAST::assignment(const Node * assignmentNode)
{
	auto node = std::make_shared<ASTNode>(ASTNodeType::assignment);
	std::vector<const Node*> children = assignmentNode->getChildren();

	// Processing identifier
	std::shared_ptr<SymbolTable> oldTable = currentTable_;
	const Node* identifierNode = lastSelectorVariable(&children, &currentTable_);
	std::string identifier = identifierNode->getValue();
	Symbol* identifierSymbol = currentTable_->getSymbol(&identifier);
    auto identAST = std::make_shared<ASTNode>(ASTNodeType::_addr, identifierSymbol);

	currentTable_ = oldTable;


	// Processing the expression
	const Node* expressionNode = children.back();
	const std::shared_ptr<ASTNode> expressionAST = expression(expressionNode);


	node->addChildBack(identAST);
	node->addChildBack(expressionAST);

	return node;
}

const std::shared_ptr<ASTNode> BuildAST::procedureCall(const Node * procedureCallNode)
{
	std::vector<const Node*> children = procedureCallNode->getChildren();
	const Node* identifierNode = children.at(0);
	std::string identifier = identifierNode->getValue();
	Symbol* procedureSymbol = currentTable_->getSymbol(&identifier);

	auto procedureCallAST = std::make_shared<ASTNode>(ASTNodeType::procedure_call, procedureSymbol);

	return procedureCallAST;
}

const std::shared_ptr<ASTNode> BuildAST::ifStatement(const Node * ifStatementNode)
{
    auto node = std::make_shared<ASTNode>(ASTNodeType::if_statement);

	std::vector<const Node*> children = ifStatementNode->getChildren();
	for (size_t i = 0; i < children.size(); i++) {
		const Node* child = children.at(i);

		if (child->getNodeType() == NodeType::expression) {
			node->addChildBack(expression(child));
		}
		else {
			node->addChildBack(statementSequence(child));
		}
	}

	return node;
}

const std::shared_ptr<ASTNode> BuildAST::whileStatement(const Node * whileStatementNode)
{
	std::vector<const Node*> children = whileStatementNode->getChildren();

	// Processing the expression
	const Node* expressionNode = children.at(0);
	const std::shared_ptr<ASTNode> expressionAST = expression(expressionNode);


	// Processing the statementSequence
	const Node* statementSequenceNode = children.at(1);
	const std::shared_ptr<ASTNode> statementSequenceAST = statementSequence(statementSequenceNode);

	// Add children to whileStatement AST node and return.
	auto whileStatementAST = std::make_shared<ASTNode>(ASTNodeType::while_statement);
	whileStatementAST->addChildBack(expressionAST);
	whileStatementAST->addChildBack(statementSequenceAST);

	return whileStatementAST;
}

const std::shared_ptr<ASTNode> BuildAST::expression(const Node * expressionNode)
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

	std::vector<std::shared_ptr<ASTNode>> nodes;
    std::shared_ptr<ASTNode> lead = simpleExpression(simpleExpressions.front());
    simpleExpressions.pop_front();
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

		auto node = std::make_shared<ASTNode>(astType);
		node->addChildBack(simpleExpression(se));
		nodes.push_back(node);
	}

    nodes.front()->addChildFront(lead);
    for (size_t i = nodes.size() - 1; i > 0; i--) {
        nodes.at(i)->addChildFront(nodes.at(i - 1));
    }

	return nodes.back();
}

const std::shared_ptr<ASTNode> BuildAST::simpleExpression(const Node * simpleExpressionNode)
{
	std::vector<const Node*> children = simpleExpressionNode->getChildren();

	// If the simple expressions consists of only one term or one term with a leading minus.
	if (children.size() == 1 || (children.size() == 2 && children.at(0)->getNodeType() == NodeType::minus)) {
		// Check for leading minus
		if (children.at(0)->getNodeType() == NodeType::minus) {
			const std::shared_ptr<ASTNode> subTerm = term(children.at(1));
			auto notInt = std::make_shared<ASTNode>(ASTNodeType::_int_not);
			notInt->addChildBack(subTerm);
			return notInt;
		} else {
			const std::shared_ptr<ASTNode> subTerm = term(children.at(0));
			return subTerm;
		}
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

	const Node* leadingMinus;
	if (operators.size() == terms.size()) {
		leadingMinus = operators.front();
		operators.pop_front();
	} else {
		leadingMinus = nullptr;
	}

	std::vector<std::shared_ptr<ASTNode>> nodes;
    std::shared_ptr<ASTNode> lead = term(terms.front());
    terms.pop_front();
	while (terms.size() > 0) {
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

		if (leadingMinus == nullptr) {
			// No leading minus in se
			auto node = std::make_shared<ASTNode>(astType);
			node->addChildBack(term(se));
			nodes.push_back(node);
		} else {
			// First term of se was leaded by a minus. Insert _int_not ast node above the term.
			auto node = std::make_shared<ASTNode>(astType);
			auto intNot = std::make_shared<ASTNode>(ASTNodeType::_int_not);
			intNot->addChildBack(term(se));
			node->addChildBack(intNot);
			nodes.push_back(node);

			leadingMinus = nullptr;
		}
	}

    nodes.front()->addChildFront(lead);
    for (size_t i = nodes.size() - 1; i > 0; i--) {
        nodes.at(i)->addChildFront(nodes.at(i - 1));
    }

	return nodes.back();
}

const std::shared_ptr<ASTNode> BuildAST::term(const Node * termNode)
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

	std::vector<std::shared_ptr<ASTNode>> nodes;
	std::shared_ptr<ASTNode> lead = factor(factors.front());
    factors.pop_front();
	while (factors.size() > 0) {
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

		auto node = std::make_shared<ASTNode>(astType);
		node->addChildBack(factor(se));
		nodes.push_back(node);
	}

	nodes.front()->addChildFront(lead);
	for (size_t i = nodes.size() - 1; i > 0; i--) {
		nodes.at(i)->addChildFront(nodes.at(i-1));
	}
	//nodes.back()->addChildBack(factor(factors.back()));

	return nodes.back();
}

const std::shared_ptr<ASTNode> BuildAST::factor(const Node * factorNode)
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

const std::shared_ptr<ASTNode> BuildAST::identifier(std::vector<const Node*>* children)
{
	std::shared_ptr<SymbolTable> oldTable = currentTable_;
	const Node* ident = lastSelectorVariable(children, &currentTable_);
	std::string identName = ident->getValue();
	Symbol* identSymbol = currentTable_->getSymbol(&identName);

	//ASTNode* node = new ASTNode(ASTNodeType::symbol, identSymbol);
	std::shared_ptr<ASTNode> node;
	if (identSymbol->getSymbolType() == SymbolType::constant) {
		node = std::make_shared<ASTNode>(ASTNodeType::_constant, identSymbol);
	} else {
		node = std::make_shared<ASTNode>(ASTNodeType::_deref, identSymbol);
	}

	currentTable_ = oldTable;
	return node;
}

const std::shared_ptr<ASTNode> BuildAST::number(const Node * numberNode)
{
	Symbol* anonymousSymbol = createAnonymousSymbol(numberNode);
	auto node = std::make_shared<ASTNode>(ASTNodeType::_constant, anonymousSymbol);
	return node;
}

const std::shared_ptr<ASTNode> BuildAST::_not(const Node * factorNode)
{
	auto node = std::make_shared<ASTNode>(ASTNodeType::_not);
	node->addChildBack(factor(factorNode));
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
