/**
 * @author fabian.klopfer@uni-konstanz.de
 */
#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H

#include <vector>
#include <string>
#include <ostream>
#include "../../util/Logger.h"

enum class NodeType : char {
	module, declarations, const_declarations,
	type_declarations, var_declarations,
	procedure_declaration, identifier, number, binary_op,
	expression, simple_expression, term, factor, record_type,
	array_type, field_list, ident_list, procedure_heading,
	procedure_body, formal_parameters, fp_section, statement_sequence,
	statement, if_statement, while_statement, acutal_parameters,
	selector, assignment, procedure_call
};

std::ostream& operator<<(std::ostream &stream, const NodeType &type);

/**
 * Generic Parse Tree class. Used for a minimal Oberon-0 compiler for now, easily extendable to other grammars
 */
class Node {

private:
    NodeType nodeType_;
    FilePos pos_;
	std::vector<Node> children_;
	std::string value_;

	void printTreeRec(std::ostream &stream, int depth) const;

public:
	explicit Node(NodeType nodeType, FilePos pos);
	explicit Node(NodeType nodeType, FilePos pos, std::string value);
	virtual ~Node();

    const NodeType getNodeType() const;

    virtual void print(std::ostream &stream) const;
	virtual void printTree(std::ostream &stream) const;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);
	
	void addChild(Node node);
	std::string getValue() const;
};

#endif //OBERON0C_AST_H