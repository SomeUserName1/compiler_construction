
#ifndef OBERON0C_FOOOOOO_H
#define OBERON0C_FOOOOOO_H

#include "../st/Symbol.h"

enum class ASTNodeType : char {
	statement_sequence, statement, if_statement, while_statement, assignment, procedure_call, symbol,
	plus, minus, div, times, or, and, leq, lt, geq, gt, eq, neq, mod, not
};

class ASTNode {
private:
	ASTNodeType nodeType_;
	std::vector<const ASTNode*> children_;
	Symbol* symbol_;

	void printTreeRec(std::ostream & stream, int depth) const;

public:
	explicit ASTNode(ASTNodeType nodeType);
	explicit ASTNode(ASTNodeType nodeType, Symbol* symbol);

	const ASTNodeType getNodeType() const;
	std::vector<const ASTNode*> getChildren() const;
	void addChild(const ASTNode* node);
	void addChilds(std::shared_ptr<std::vector<const ASTNode*>> nodes);

	virtual void printTree(std::ostream &stream) const;
	friend std::ostream& operator<<(std::ostream &stream, const ASTNodeType &type);
	friend std::ostream& operator<<(std::ostream &stream, const ASTNode &node);
};

#endif //OBERON0C_FOOOOOO_H