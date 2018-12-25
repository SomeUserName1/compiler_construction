/*
 * Header file of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H


#include <vector>
#include <string>
#include <ostream>
#include "../../util/Logger.h"
#include "../st/SymbolTable.h"

enum class NodeType : char {
	module, declarations, const_declarations,
	type_declarations, var_declarations,
	procedure_declaration, identifier, number, binary_op,
	expression, simple_expression, term, factor, type, record_type,
	array_type, field_list, ident_list, procedure_heading,
	procedure_body, formal_parameters, fp_section, statement_sequence,
	statement, if_statement, else_if, else_, while_statement, acutal_parameters,
	selector, assignment, procedure_call,
	plus, minus, div, times, or, and, leq, lt, geq, gt, eq, neq, mod
};

std::ostream& operator<<(std::ostream &stream, const NodeType &type);

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;
	std::vector<const Node*> children_;
	std::string value_;
	std::shared_ptr<SymbolTable> symbolTable_;

	void printTreeRec(std::ostream &stream, int depth) const;

public:
	explicit Node(NodeType nodeType, FilePos pos, std::shared_ptr<SymbolTable> symbolTable);
	explicit Node(NodeType nodeType, FilePos pos, std::string value, std::shared_ptr<SymbolTable> symbolTable);
	virtual ~Node();

    const NodeType getNodeType() const;
    const FilePos getFilePos() const;
	std::vector<const Node*> getChildren() const;
	std::shared_ptr<SymbolTable> getSymbolTable() const;
	void addChild(const Node* node);
	void addChilds(std::shared_ptr<std::vector<const Node*>> nodes);
	std::string getValue() const;
	bool isBinaryOp() const;
	void setSymbolTable(std::shared_ptr<SymbolTable> symbolTable);

    virtual void print(std::ostream &stream) const;
	virtual void printTree(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);
	

};

#endif //OBERON0C_AST_H