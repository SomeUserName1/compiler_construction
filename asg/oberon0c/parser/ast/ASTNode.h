
#ifndef OBERON0C_FOOOOOO_H
#define OBERON0C_FOOOOOO_H

#include "../st/Symbol.h"
#include <list>

enum class ASTNodeType : char {
	statement_sequence, statement, if_statement, while_statement, assignment, procedure_call, symbol,
	plus, minus, _int_not, div, times, _or, _and, leq, lt, geq, gt, eq, neq, mod, _not, _constant, _deref, _addr
};

std::ostream& operator<<(std::ostream &stream, const ASTNodeType &type);

class ASTNode {
private:
	ASTNodeType nodeType_;
	std::list<std::shared_ptr<ASTNode>> children_;
	Symbol* symbol_;

	void printTreeRec(std::ostream & stream, int depth) const;

public:
	explicit ASTNode(ASTNodeType nodeType);
	explicit ASTNode(ASTNodeType nodeType, Symbol* symbol);

	const ASTNodeType getNodeType() const;
    std::list<std::shared_ptr<ASTNode>> getChildren() const;
    void addChildFront(std::shared_ptr<ASTNode> node);
    void addChildBack(std::shared_ptr<ASTNode> node);
	void addChilds(std::shared_ptr<std::vector<std::shared_ptr<ASTNode>>> nodes);

	void printTree(std::ostream &stream) const;
	//friend std::ostream& operator<<(std::ostream &stream, const ASTNodeType &type);
	friend std::ostream& operator<<(std::ostream &stream, const ASTNode &node);

	Symbol* getSymbol() const;
};

#endif //OBERON0C_FOOOOOO_H