

//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include <stdexcept>
#include <IdentToken.h>
#include <NumberToken.h>
#include <utility>
#include "Parser.h"

Parser::Parser(Scanner *scanner, Logger *logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Node> Parser::parse() {
    std::unique_ptr<const Node>
    parse_tree(module());

    std::cout << *parse_tree << std::endl;

    return parse_tree;
}

const Node* Parser::ident() {
	word = scanner_->nextToken();

	if (word->getType() != TokenType::const_ident) {
        std::string s = std::string("Expected identifier. ident()");
		fail(s);
	}
    auto* identifier = (IdentToken*) &*word;
    Node* node = new Node(NodeType::identifier, word->getPosition(), identifier->getValue());
	
	return node;
}

const Node* Parser::module() {

	word = scanner_->nextToken();
	if (word->getType() != TokenType::kw_module) {
	    std::string s = std::string("Expected module declaration. module()");
		fail(s);
	}

	// Module declaration
	Node* moduleNode = new Node(NodeType::module, word->getPosition());
	const Node* identifier = ident();
	moduleNode->addChild(*identifier);
	semicolon_t();

	// Declarations
	moduleNode->addChild(*declarations());

	// Optional: Begin (main method)
	if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
		begin_t();
		moduleNode->addChild(*statement_sequence());
	}

	//End
	end_t();
	if (identifier->getValue() != ident()->getValue()) {
	    std::string s = std::string("Expected module identifier module()");
		fail(s);
	}
	point_t();

	return moduleNode;
}

const Node* Parser::declarations() {
	Node* node = new Node(NodeType::declarations, word->getPosition());

	// CONSTs
	if (scanner_->peekToken()->getType() == TokenType::kw_const) {
		const_t();

		while (scanner_->peekToken()->getType() == TokenType::const_ident) {
			node->addChild(*const_declarations());
		}
	}

	// TYPEs
	if (scanner_->peekToken()->getType() == TokenType::kw_type) {
		type_t();
		while (scanner_->peekToken()->getType() == TokenType::const_ident) {
			node->addChild(*type_declarations());
		}
	}

	// VARs
	if (scanner_->peekToken()->getType() == TokenType::kw_var) {
		var_t();
		while (scanner_->peekToken()->getType() == TokenType::const_ident) {
			node->addChild(*var_declarations());
		}
	}


	// Optional Procedures
	while (scanner_->peekToken()->getType() == TokenType::kw_procedure) {
		node->addChild(*procedure_declaration());
	}

	return node;
}

const Node* Parser::const_declarations() {
	Node* node = new Node(NodeType::const_declarations, word->getPosition());

    node->addChild(*ident());
	equals_symbol_t();
	node->addChild(*expression());
	semicolon_t();

	return node;
}

const Node* Parser::type_declarations() {
	Node* node = new Node(NodeType::type_declarations, word->getPosition());

    node->addChild(*ident());
	equals_symbol_t();
	node->addChild(*type());
	semicolon_t();

	return node;
}

const Node* Parser::var_declarations() {
	Node* node = new Node(NodeType::var_declarations, word->getPosition());

	ident_list();
	double_colon_t();
	node->addChild(*type());
	semicolon_t();

	return node;
}

const Node* Parser::procedure_declaration() {
	Node* node = new Node(NodeType::procedure_declaration, word->getPosition());

	node->addChild(*procedure_heading());
	semicolon_t();
	node->addChild(*procedure_body());

	return node;
}

const Node* Parser::expression() {
	Node* node = new Node(NodeType::expression, word->getPosition());

	node->addChild(*simple_expression());

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::op_eq || type == TokenType::op_neq
		|| type == TokenType::op_lt || type == TokenType::op_leq
		|| type == TokenType::op_gt || type == TokenType::op_geq) {
		node->addChild(*binary_op());
		node->addChild(*simple_expression());
	}

	return node;
}

const Node* Parser::simple_expression() {
	Node* node = new Node(NodeType::simple_expression, word->getPosition());

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::op_plus || type == TokenType::op_minus) {
		node->addChild(*binary_op());
	}
	node->addChild(*term());

	type = scanner_->peekToken()->getType();
	while (type == TokenType::op_plus || type == TokenType::op_minus
		|| type == TokenType::op_or) {
		node->addChild(*binary_op());
		node->addChild(*term());

		type = scanner_->peekToken()->getType();
	}


	return node;
}

const Node* Parser::term() {
	Node* node = new Node(NodeType::term, word->getPosition());

	node->addChild(*factor());

	TokenType type = scanner_->peekToken()->getType();
	while (type == TokenType::op_times || type == TokenType::op_div
		|| type == TokenType::op_mod || type == TokenType::op_and) {
		node->addChild(*binary_op());
		node->addChild(*factor());

		type = scanner_->peekToken()->getType();
	}

	return node;
}

const Node* Parser::number() {
    word = scanner_->nextToken();

    if (word->getType() != TokenType::const_number) {
        std::string s = std::string("Expected number.");
        fail(s);
    }
    auto* number = (NumberToken*) &*word;
    Node* node = new Node(NodeType::number, word->getPosition(), std::to_string(number->getValue()));

    return node;
}


const Node* Parser::factor() {
	Node* node = new Node(NodeType::factor, word->getPosition());


	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		node->addChild(*ident());
		node->addChild(*selector());
	}
	else if (type == TokenType::const_number) {
        node->addChild(*number());
	}
	else if (type == TokenType::lparen) {
		lparen_t();
		node->addChild(*expression());
		rparen_t();
	}
	else if (type == TokenType::op_not) {
		not_t();
		node->addChild(*factor());
	}
	else {
		word = scanner_->nextToken();
		std::string s = std::string("Expected a factor but was something else factor()");
		fail(s);
	}

	return node;
}

const Node* Parser::type() {
	Node* node = new Node(NodeType::factor, word->getPosition());

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		node->addChild(*ident());
	}
	else if (type == TokenType::kw_array) {
		node->addChild(*array_type());
	}
	else if (type == TokenType::kw_record) {
		node->addChild(*record_type());
	}
	else {
	    std::string s = std::string("Unknown error (expected type) type()");
		fail(s);
	}

	return node;
}

const Node* Parser::array_type() {
	Node* node = new Node(NodeType::array_type, word->getPosition());

	array_t();
	node->addChild(*expression());
	of_t();
	node->addChild(*type());

	return node;
}

const Node* Parser::record_type() {
	Node* node = new Node(NodeType::record_type, word->getPosition());

	record_t();
	node->addChild(*field_list());

	TokenType type = scanner_->peekToken()->getType();
	while (type == TokenType::semicolon) {
		semicolon_t();
		node->addChild(*field_list());

		type = scanner_->peekToken()->getType();
	}
	end_t();

	return node;
}

const Node* Parser::field_list() {
	Node* node = new Node(NodeType::field_list, word->getPosition());

	TokenType t_type = scanner_->peekToken()->getType();
	if (t_type == TokenType::const_ident) {
		node->addChild(*ident_list());
		double_colon_t();
		node->addChild(*type());
	}
	return node;
}

const Node* Parser::ident_list() {
	Node* node = new Node(NodeType::ident_list, word->getPosition());

	node->addChild(*ident());

	while (scanner_->peekToken()->getType() == TokenType::comma) {
		comma_t();
		node->addChild(*ident());
	}

	return node;
}

const Node* Parser::procedure_heading() {
	Node* node = new Node(NodeType::procedure_heading, word->getPosition());

	procedure_t();
	node->addChild(*ident());

	if (scanner_->peekToken()->getType() == TokenType::lparen) {
		node->addChild(*formal_parameters());
	}
	return node;
}

const Node* Parser::procedure_body()
{
	Node* node = new Node(NodeType::procedure_body, word->getPosition());

	node->addChild(*declarations());

	if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
		begin_t();
		node->addChild(*statement_sequence());
	}
	end_t();
	node->addChild(*ident());
	semicolon_t();

	return node;
}

const Node* Parser::formal_parameters()
{
	Node* node = new Node(NodeType::formal_parameters, word->getPosition());

	lparen_t();

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::kw_var || type == TokenType::const_ident) {
		node->addChild(*fp_section());
		while (scanner_->peekToken()->getType() == TokenType::semicolon) {
			semicolon_t();
			node->addChild(*fp_section());
		}
	}
	rparen_t();

	return node;
}

const Node* Parser::fp_section()
{
	Node* node = new Node(NodeType::fp_section, word->getPosition());

	if (scanner_->peekToken()->getType() == TokenType::kw_var) {
		var_t();
	}
	node->addChild(*ident_list());
	double_colon_t();
	node->addChild(*type());

	return node;
}

const Node* Parser::statement_sequence()
{
	Node* node = new Node(NodeType::statement_sequence, word->getPosition());

	node->addChild(*statement());
	while (scanner_->peekToken()->getType() == TokenType::semicolon) {
		semicolon_t();
		node->addChild(*statement());
	}
	return node;
}

const Node* Parser::statement()
{ //TODO
	Node* node = new Node(NodeType::statement, word->getPosition());

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		node->addChild(*A());
	}
	else if (type == TokenType::kw_if) {
		node->addChild(*if_statement());
	}
	else if (type == TokenType::kw_while) {
		node->addChild(*while_statement());
	}


	return node;

}

// Implements Assignment & ProcedureCall
const Node* Parser::A()
{
	Node* node;

	const Node* identifier = ident();
	const Node* select = selector();

	if (scanner_->peekToken()->getType() == TokenType::op_becomes) {
		node = new Node(NodeType::assignment, word->getPosition());
		node->addChild(*identifier);
		node->addChild(*select);
		becomes_t();
		node->addChild(*expression());
	}
	else {
		node = new Node(NodeType::procedure_call, word->getPosition());
		node->addChild(*identifier);
		node->addChild(*select);

		if (scanner_->peekToken()->getType() == TokenType::lparen) {
			node->addChild(*actual_parameters());
		}
	}

	return node;
}

const Node* Parser::if_statement()
{
	Node* node = new Node(NodeType::if_statement, word->getPosition());

	if_t();
	node->addChild(*expression());
	then_t();
	node->addChild(*statement_sequence());

	while (scanner_->peekToken()->getType() == TokenType::kw_elsif) {
		elseif_t();
		node->addChild(*expression());
		then_t();
		node->addChild(*statement_sequence());
	}
	if (scanner_->peekToken()->getType() == TokenType::kw_else) {
		else_t();
		node->addChild(*statement_sequence());
	}
	end_t();

	return node;
}

const Node* Parser::while_statement()
{
	Node* node = new Node(NodeType::while_statement, word->getPosition());

	while_t();
	node->addChild(*expression());
	do_t();
	node->addChild(*statement_sequence());
	end_t();

	return node;
}

const Node* Parser::actual_parameters()
{
	Node* node = new Node(NodeType::acutal_parameters, word->getPosition());

	lparen_t();
	if (scanner_->peekToken()->getType() != TokenType::rparen) {
		node->addChild(*expression());
		while (scanner_->peekToken()->getType() == TokenType::comma) {
			comma_t();
			node->addChild(*expression());
		}
	}
	rparen_t();

	return node;
}

const Node* Parser::selector()
{
	Node* node = new Node(NodeType::selector, word->getPosition());

	while (scanner_->peekToken()->getType() == TokenType::period
			|| scanner_->peekToken()->getType() == TokenType::lbrack) {
		if (scanner_->peekToken()->getType() == TokenType::period) {
			point_t();
			node->addChild(*ident());
		}
		else {
			lbrack_t();
			node->addChild(*expression());
			rbrack_t();
		}
	}

	return node;
}

void Parser::fail(std::string &msg) {
    std::stringstream ss;
    ss << " got " << *word << std::endl;
    msg = msg + ss.str();

	logger_->error(word->getPosition(), msg);
	throw  std::invalid_argument("You failed!" + msg);
}

void Parser::semicolon_t() {
    std:: string s = std::string("semicolon expected");
	decideToken(TokenType::semicolon, s);
}

void Parser::comma_t() {
    std::string s = std::string("comma expected");
	decideToken(TokenType::comma, s);
}

void Parser::begin_t() {
    std::string s = std::string("semicolon expected");
	decideToken(TokenType::kw_begin, s);
}

void Parser::end_t() {
    std::string s =  std::string("End expected end_t()");
	decideToken(TokenType::kw_end, s);
}

void Parser::point_t() {
    std::string s = std::string("\".\" expected");
	decideToken(TokenType::period, s);
}

void Parser::equals_symbol_t() {
    std::string s = std::string("Expected assignment \"=\"");
	decideToken(TokenType::op_eq, s);
}

void Parser::double_colon_t() {
    std::string s = std::string("Expected \":\"");
	decideToken(TokenType::colon, s);
}

void Parser::lparen_t() {
    std::string s = std::string("Expected \"(\"");
	decideToken(TokenType::lparen, s);
}

void Parser::rparen_t() {
    std::string s = std::string("Expected \")\"");
	decideToken(TokenType::rparen, s);
}

void Parser::lbrack_t() {
    std::string s = std::string("Expected \"[\"");
	decideToken(TokenType::lbrack, s);
}

void Parser::rbrack_t() {
    std::string s = std::string("Expected \"]\"");
	decideToken(TokenType::rbrack, s);
}

void Parser::not_t() {
    std::string s = std::string("Expected \"]\"");
	decideToken(TokenType::op_not, s);
}

void Parser::array_t() {
    std::string s = std::string("Expected ARRAY keyword");
	decideToken(TokenType::kw_array, s);
}

void Parser::of_t() {
    std::string s = std::string("Expected OF keyword");
	decideToken(TokenType::kw_of, s);
}

void Parser::record_t() {
    std::string s = std::string("Expected RECORD keyword");
	decideToken(TokenType::kw_record, s);
}

void Parser::procedure_t() {
    std::string s = std::string("Expected PROCEDURE keyword");
	decideToken(TokenType::kw_procedure, s);
}

void Parser::const_t() {
    std::string s = std::string("Expected CONST keyword");
	decideToken(TokenType::kw_const, s);
}

void Parser::type_t() {
    std::string s = std::string("Expected TYPE keyword");
	decideToken(TokenType::kw_type, s);
}

void Parser::var_t() {
    std::string s = std::string("Expected VAR keyword");
	decideToken(TokenType::kw_var, s);
}

void Parser::becomes_t() {
    std::string s = std::string("Expected \":=\" operator");
	decideToken(TokenType::op_becomes, s);
}

void Parser::if_t() {
    std::string s = std::string("Expected IF keyword");
	decideToken(TokenType::kw_if, s);
}

void Parser::then_t() {
    std::string s = std::string("Expected THEN keyword");
	decideToken(TokenType::kw_then, s);
}

void Parser::else_t() {
    std::string s = std::string("Expected ELSE keyword");
	decideToken(TokenType::kw_else, s);
}

void Parser::elseif_t() {
    std::string s = std::string("Expected ELSEIF keyword");
	decideToken(TokenType::kw_elsif, s);
}

void Parser::while_t() {
    std::string s = std::string("Expected WHILE keyword");
	decideToken(TokenType::kw_while, s);
}

void Parser::do_t() {
    std::string s =  std::string("Expected DO keyword");
	decideToken(TokenType::kw_do, s);
}

const Node* Parser::binary_op() {
	word = scanner_->nextToken();

	TokenType type = word->getType();
	if (type == TokenType::op_eq) {
		return new Node(NodeType::binary_op, word->getPosition(), "=");
	}
	else if (type == TokenType::op_neq) {
		return new Node(NodeType::binary_op, word->getPosition(), "#");
	}
	else if (type == TokenType::op_lt) {
		return new Node(NodeType::binary_op, word->getPosition(), "<");
	}
	else if (type == TokenType::op_leq) {
		return new Node(NodeType::binary_op, word->getPosition(), "<=");
	}
	else if (type == TokenType::op_gt) {
		return new Node(NodeType::binary_op, word->getPosition(), ">");
	}
	else if (type == TokenType::op_geq) {
		return new Node(NodeType::binary_op, word->getPosition(), ">=");
	}
	else if (type == TokenType::op_plus) {
		return new Node(NodeType::binary_op, word->getPosition(), "+");
	}
	else if (type == TokenType::op_minus) {
		return new Node(NodeType::binary_op, word->getPosition(), "-");
	}
	else if (type == TokenType::op_or) {
		return new Node(NodeType::binary_op, word->getPosition(), "OR");
	}
	else if (type == TokenType::op_times) {
		return new Node(NodeType::binary_op, word->getPosition(), "*");
	}
	else if (type == TokenType::op_div) {
		return new Node(NodeType::binary_op, word->getPosition(), "DIV");
	}
	else if (type == TokenType::op_mod) {
		return new Node(NodeType::binary_op, word->getPosition(), "MOD");
	}
	else if (type == TokenType::op_and) {
		return new Node(NodeType::binary_op, word->getPosition(), "&");
	}

	std::string s = std::string("Expected binary operator in method binary op");
	fail(s);

	return nullptr;
}

void Parser::decideToken(TokenType type, std::string &errormsg) {
	word = scanner_->nextToken();
	if (word->getType() != type) {
		fail(errormsg);
	}
}
