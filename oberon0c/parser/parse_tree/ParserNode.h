/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * Generic node to convert the code to a tree structure and do syntax checking during construction
 */
#ifndef OBERON0C_AST_H
#define OBERON0C_AST_H

#include <Node.h>

enum class ParserNodeType : char {
	module, declarations, const_declaration,
	type_declaration, var_declaration,
	procedure_declaration, identifier, number, type, binary_op,
	expression, simple_expression, term, factor, record_type,
	array_type, field_list, ident_list, procedure_heading,
	procedure_body, formal_parameters, fp_section, statement_sequence,
	statement, if_statement, while_statement, acutal_parameters,
	selector, assignment, procedure_call, syntax_error, terminal_token
};

std::ostream& operator<<(std::ostream &stream, const ParserNodeType &type);

/**
 * Generic Parse Tree class. Used for a minimal Oberon-0 compiler for now, easily extendable to other grammars
 */
class ParserNode : public Node {

private:
    ParserNodeType nodeType_;
    FilePos pos_;
	std::string value_;

	void printTreeRec(std::ostream &stream, int depth) const override;

public:
	ParserNode(ParserNodeType nodeType, FilePos pos);
	ParserNode(ParserNodeType nodeType, FilePos pos, std::string value);
	~ParserNode() override;

    const ParserNodeType getParserNodeType() const;

    void print(std::ostream &stream) const override;
    friend std::ostream& operator<<(std::ostream &stream, const ParserNode &node);

	std::string getValue() const;
};

#endif //OBERON0C_AST_H