
#include "Node.h"
#include "ASTNode.h"

#ifndef OBERON0C_ASTBUILD_H
#define OBERON0C_ASTBUILD_H


class BuildAST {
private:
	std::shared_ptr<SymbolTable> currentTable_;
	std::shared_ptr<Node> parseTree_;
	size_t anonymousCounter_;

public:
	BuildAST(std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<Node> parseTree);

	void build();
	void declarations(const Node* declarationsNode);
	const std::shared_ptr<ASTNode> statementSequence(const Node* statementSequenceNode);
	void procedureDeclaration(const Node* procedureDeclarationNode);
	const std::shared_ptr<ASTNode> statement(const Node* statementNode);
	const std::shared_ptr<ASTNode> assignment(const Node* assignmentNode);
	const std::shared_ptr<ASTNode> procedureCall(const Node* procedureCallNode);
	const std::shared_ptr<ASTNode> ifStatement(const Node* ifStatementNode);
	const std::shared_ptr<ASTNode> whileStatement(const Node* whileStatementNode);
	const std::shared_ptr<ASTNode> expression(const Node* expressionNode);
	const std::shared_ptr<ASTNode> simpleExpression(const Node* simpleExpressionNode);
	const std::shared_ptr<ASTNode> term(const Node* termNode);
	const std::shared_ptr<ASTNode> factor(const Node* factorNode);
	const std::shared_ptr<ASTNode> identifier(std::vector<const Node*>* children);
	const std::shared_ptr<ASTNode> number(const Node* numberNode);
	const std::shared_ptr<ASTNode> _not(const Node* factorNode);


	const Node* lastSelectorVariable(std::vector<const Node*>* children, std::shared_ptr<SymbolTable>* table);
	Symbol* createAnonymousSymbol(const Node* numberNode);
};


#endif //OBERON0C_ASTBUILD_H