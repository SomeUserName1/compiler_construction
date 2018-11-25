//
// Created by Michael Grossniklaus on 2/2/18.
//

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H


#include "../scanner/Scanner.h"
#include "ast/Node.h"

class Parser
{

private:
    Scanner *scanner_;
    Logger *logger_;

	std::unique_ptr<const Token> word;

    const Node* ident();

    const Node* module();
    const Node* declarations();
    const Node* const_declarations();
    const Node* type_declarations();
    const Node* var_declarations();
    const Node* procedure_declaration();
    const Node* expression();
    const Node* simple_expression();
    const Node* term();
    const Node* factor();
    const Node* type();
    const Node* array_type();
    const Node* record_type();
    const Node* field_list();
    const Node* ident_list();
    const Node* procedure_heading();
    const Node* procedure_body();
    const Node* formal_parameters();
    const Node* fp_section();
    const Node* statement_sequence();
    const Node* statement();
    const Node* if_statement();
    const Node* while_statement();
    const Node* actual_parameters();
    const Node* selector();

	void fail(FilePos pos, std::string &msg);
	void semicolon();
	bool begin();
	void end();
	void point();
	void equals_symbol();
	void double_colon();
	const Node* binary_op();
	const Node* A();
	const Node* B();

	void decideToken(TokenType type, std::string &errormsg);


public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<const Node> parse();
};


#endif //OBERON0C_PARSER_H
