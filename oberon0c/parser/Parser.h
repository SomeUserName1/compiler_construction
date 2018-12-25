/**
 * @authors fabian.klopfer@uni-konstanz.de, daniel.metzger@uni-konstanz.de
 * 
 * Check the syntax of the code.
 * Iff the code is syntactically correct it's converted into a parse tree and the type checker is invoked to do
 * semantic analysis and to build the AST
 * 
 */
#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include <iostream>
#include <stdexcept>
#include <IdentToken.h>
#include <NumberToken.h>
#include <utility>
#include <type_traits>

#include <Lexer.h>
#include <parse_tree/ParserNode.h>


class Parser
{

private:
    std::shared_ptr<Lexer> scanner_;
    std::shared_ptr<Logger> logger_;
	std::unique_ptr<const Token> word;
	bool _state = true;

	// Non-Terminals
    const std::unique_ptr<ParserNode> module();
    const std::shared_ptr<ParserNode> declarations();
    const std::shared_ptr<ParserNode> const_declarations();
    const std::shared_ptr<ParserNode> type_declarations();
    const std::shared_ptr<ParserNode> var_declarations();
    const std::shared_ptr<ParserNode> procedure_declaration();
    const std::shared_ptr<ParserNode> expression();
    const std::shared_ptr<ParserNode> simple_expression();
    const std::shared_ptr<ParserNode> term();
    const std::shared_ptr<ParserNode> factor();
    const std::shared_ptr<ParserNode> type();
    const std::shared_ptr<ParserNode> array_type();
    const std::shared_ptr<ParserNode> record_type();
    const std::shared_ptr<ParserNode> field_list();
    const std::shared_ptr<ParserNode> ident_list();
    const std::shared_ptr<ParserNode> procedure_heading();
    const std::shared_ptr<ParserNode> procedure_body();
    const std::shared_ptr<ParserNode> formal_parameters();
    const std::shared_ptr<ParserNode> fp_section();
    const std::shared_ptr<ParserNode> statement_sequence();
    const std::shared_ptr<ParserNode> statement();
    const std::shared_ptr<ParserNode> if_statement();
    const std::shared_ptr<ParserNode> while_statement();
    const std::shared_ptr<ParserNode> actual_parameters();
    const std::shared_ptr<ParserNode> selector();
    const std::shared_ptr<ParserNode> identifier();
	const std::shared_ptr<ParserNode> number();

	const std::shared_ptr<ParserNode> binary_op();
	const std::shared_ptr<ParserNode> id_sel();

	const std::shared_ptr<ParserNode> decideToken(TokenType type);

	void fail(std::string &msg);

public:
    explicit Parser(Lexer* scanner, Logger* logger);
    ~Parser();
    const std::unique_ptr<const ParserNode> parse();
};


#endif //OBERON0C_PARSER_H
