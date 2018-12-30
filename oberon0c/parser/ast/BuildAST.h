
#include "Node.h"
#include "ASTNode.h"

#ifndef OBERON0C_ASTBUILD_H
#define OBERON0C_ASTBUILD_H


class BuildAST {
private:
	std::shared_ptr<SymbolTable> currentTable_;
	std::shared_ptr<Node> parseTree_;

public:
	BuildAST(std::shared_ptr<SymbolTable> symbolTable, std::shared_ptr<Node> parseTree);

	void build();
	void declarations(const Node* declarationsNode);
	const ASTNode* statementSequence(const Node* statementSequenceNode);
	void procedureDeclaration(const Node* procedureDeclarationNode);
	const ASTNode* statement(const Node* statementNode);
	const ASTNode* assignment(const Node* assignmentNode);
	const ASTNode* procedureCall(const Node* procedureCallNode);
	const ASTNode* ifStatement(const Node* ifStatementNode);
	const ASTNode* whileStatement(const Node* whileStatementNode);
	const ASTNode* expression(const Node* expressionNode);
	const ASTNode* simpleExpression(const Node* simpleExpressionNode);


	const Node* lastSelectorVariable(std::vector<const Node*>* children, std::shared_ptr<SymbolTable>* table);
};


#endif //OBERON0C_ASTBUILD_H