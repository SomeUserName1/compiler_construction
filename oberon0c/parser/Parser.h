//
// Created by Michael Grossniklaus on 2/2/18.
//

#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include <vector>
#include "../scanner/Scanner.h"
#include "ast/Node.h"
#include "st/SymbolTable.h"

class Parser
{

private:
    Scanner *scanner_;
    Logger *logger_;

	std::unique_ptr<const Token> word;

	std::vector<SymbolTable> symbolTables_;
	SymbolTable * currentTable_;



	// None-Terminals
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
    const Node* ident();
	const Node* number();

	// Added non-terminals
	const Node* binary_op();
	const Node* A();

	//Terminals
	void module_t();
	void semicolon_t();
	void comma_t();
	void begin_t();
	void end_t();
	void point_t();
	void equals_symbol_t();
	void double_colon_t();
	void lparen_t();
	void rparen_t();
	void lbrack_t();
	void rbrack_t();
	void not_t();
	void array_t();
	void of_t();
	void record_t();
	void procedure_t();
	void const_t();
	void type_t();
	void var_t();
	void becomes_t();
	void if_t();
	void then_t();
	void else_t();
	void elseif_t();
	void while_t();
	void do_t();
	// Decides given terminals
	void decideToken(TokenType type, std::string &errormsg);

	// Generic error handler
	void failToken(std::string &msg);
	void failSymbol(std::string &msg);
	void failUndeclaredSymbol(Symbol * undeclaredSymbol, Node *identifier);
	void failIfNotAType(Symbol *identifier);
	void failSymbolExists(Symbol * symbol);

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<const Node> parse();
};


#endif //OBERON0C_PARSER_H
