/**
 * @authors fabian.klopfer@uni-konstanz.de, daniel.metzger@uni-konstanz.de
 */
#ifndef OBERON0C_PARSER_H
#define OBERON0C_PARSER_H

#include <iostream>
#include <stdexcept>
#include <IdentToken.h>
#include <NumberToken.h>
#include <utility>

#include <Scanner.h>
#include <parse_tree/Node.h>


class Parser
{

private:
    std::shared_ptr<Scanner> scanner_;
    std::shared_ptr<Logger> logger_;
	std::unique_ptr<const Token> word;

	// Non-Terminals
    const std::unique_ptr<Node> module();
    const std::shared_ptr<Node> declarations();
    const std::shared_ptr<Node> const_declarations();
    const std::shared_ptr<Node> type_declarations();
    const std::shared_ptr<Node> var_declarations();
    const std::shared_ptr<Node> procedure_declaration();
    const std::shared_ptr<Node> expression();
    const std::shared_ptr<Node> simple_expression();
    const std::shared_ptr<Node> term();
    const std::shared_ptr<Node> factor();
    const std::shared_ptr<Node> type();
    const std::shared_ptr<Node> array_type();
    const std::shared_ptr<Node> record_type();
    const std::shared_ptr<Node> field_list();
    const std::shared_ptr<Node> ident_list();
    const std::shared_ptr<Node> procedure_heading();
    const std::shared_ptr<Node> procedure_body();
    const std::shared_ptr<Node> formal_parameters();
    const std::shared_ptr<Node> fp_section();
    const std::shared_ptr<Node> statement_sequence();
    const std::shared_ptr<Node> statement();
    const std::shared_ptr<Node> if_statement();
    const std::shared_ptr<Node> while_statement();
    const std::shared_ptr<Node> actual_parameters();
    const std::shared_ptr<Node> selector();
    const std::shared_ptr<Node> identifier();
	const std::shared_ptr<Node> number();

	const std::shared_ptr<Node> binary_op();
	const std::shared_ptr<Node> IdSel();

	void decideToken(TokenType type);

	void fail(std::string &msg);

public:
    explicit Parser(Scanner* scanner, Logger* logger);
    ~Parser();
    const std::unique_ptr<const Node> parse();
};


#endif //OBERON0C_PARSER_H
