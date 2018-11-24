/*
 * Header file of the base class of all AST nodes used by the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 2/2/18.
 */

#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H


#include <list>
#include <string>
#include <ostream>
#include "../../util/Logger.h"

enum class NodeType : char {
	module, declarations, const_declarations,
	type_declarations, var_declarations,
	procedure_declaration, identifier, binary_op,
	expression, simple_expression, term, factor
};

class Node {

private:
    NodeType nodeType_;
    FilePos pos_;
	std::list<Node> childs_;
	std::string value_;

public:
	explicit Node(NodeType nodeType, FilePos pos);
	explicit Node(NodeType nodeType, FilePos pos, std::string value);
	virtual ~Node();

    const NodeType getNodeType() const;
    const FilePos getFilePos() const;

    virtual void print(std::ostream &stream);
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);
	
	void addChild(Node node);
	std::string getValue() {
		return value_;
	}
};

#endif //OBERON0C_AST_H