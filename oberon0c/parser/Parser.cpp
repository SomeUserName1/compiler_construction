//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include "Parser.h"

Parser::Parser(Scanner *scanner, Logger *logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<const Node> Parser::parse() {
	return std::unique_ptr<const Node>(module());
}

const Node * Parser::ident()
{
	word = scanner_->nextToken();

	if (word->getType() != TokenType::const_ident) {
		fail(word->getPosition(), std::string("Expected identifier."));
	}

	//	 IdentToken ident = dynamic_cast<IdentToken*> (word);
	//	 IdentToken * ident = dynamic_cast<IdentToken&>(word);

	return &Node(NodeType::identifier, word->getPosition(), std::string("I'm an identifier!"));
}

const Node * Parser::module()
{
	word = scanner_->nextToken();
	if (word->getType() != TokenType::kw_module) {
		fail(word->getPosition(), std::string("Expected module declaration."));
	}

	// Module declaration
	Node * moduleNode = &Node(NodeType::module, word->getPosition());
	const Node * identifier = ident();
	moduleNode->addChild(*identifier);
	semicolon();

	// Declarations
	moduleNode->addChild(*declarations());

	// Optional: Begin (main method)
	if (begin()) {
		moduleNode->addChild(*statement_sequence());
	}

	//End
	end();
	if (identifier->getValue() != ident()->getValue()) {
		fail(word->getPosition(), std::string("Expected module identifier"));
	}
	point();

	return moduleNode;
}

const Node * Parser::declarations()
{
	Node * node = &Node(NodeType::declarations, word->getPosition());

	word = scanner_->nextToken();
	if (word->getType() == TokenType::kw_const) {
		while (scanner_->peekToken()->getType() != TokenType::kw_type) {
			node->addChild(*const_declarations());
		}

		word = scanner_->nextToken();
	}

	if (word->getType() == TokenType::kw_type) {
		while (scanner_->peekToken()->getType() != TokenType::kw_var) {
			node->addChild(*type_declarations());
		}
		word = scanner_->nextToken();
	}

	if (word->getType() == TokenType::kw_var) {
		const Token * peek = scanner_->peekToken();
		while (peek->getType() != TokenType::kw_procedure && peek->getType() != TokenType::kw_end
			&& peek->getType() != TokenType::kw_begin) {
			node->addChild(*type_declarations());
		}
	}

	// Optional Procedures
	const Token * peek = scanner_->peekToken();
	while (peek->getType() == TokenType::kw_procedure) {
		node->addChild(*procedure_declaration());
	}

	return node;
}

const Node * Parser::const_declarations()
{
	Node * node = &Node(NodeType::const_declarations, word->getPosition());

	// TODO handle string
	ident();
	equals_symbol();
	node->addChild(*expression());
	semicolon();

	return node;
}

const Node * Parser::type_declarations()
{
	Node * node = &Node(NodeType::type_declarations, word->getPosition());

	// TODO handle string
	ident();
	equals_symbol();
	node->addChild(*type());
	semicolon();

	return node;
}

const Node * Parser::var_declarations()
{
	Node * node = &Node(NodeType::var_declarations, word->getPosition());

	// TODO handle string
	ident_list();
	double_colon();
	node->addChild(*type());
	semicolon();

	return node;
}

const Node * Parser::procedure_declaration()
{
	Node * node = &Node(NodeType::procedure_declaration, word->getPosition());

	node->addChild(*procedure_heading());
	semicolon();
	node->addChild(*procedure_body());

	return node;
}

const Node * Parser::expression()
{
	Node * node = &Node(NodeType::expression, word->getPosition());

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

const Node * Parser::simple_expression()
{
	Node * node = &Node(NodeType::simple_expression, word->getPosition());

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

const Node * Parser::term()
{
	Node * node = &Node(NodeType::term, word->getPosition());

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

const Node * Parser::factor()
{
	Node * node = &Node(NodeType::factor, word->getPosition());


	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		node->addChild(*ident());
		node->addChild(*selector());
	}
	else if (type == TokenType::const_number) {
		// TODO Number extracten
	}
	else if (type == TokenType::lparen) {
		decideToken(TokenType::lparen, std::string("Expected \"(\""));
		node->addChild(*expression());
		decideToken(TokenType::rparen, std::string("Expected \")\""));
	}
	else if (type == TokenType::op_not) {
		decideToken(TokenType::op_not, std::string("Expected \"~\""));
		node->addChild(*factor());
	}
	else {
		word = scanner_->nextToken();
		fail(word->getPosition(), std::string("Expected a factor but was something else"));
	}

	return node;
}

const Node * Parser::type()
{
	Node * node = &Node(NodeType::factor, word->getPosition());

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
		fail(word->getPosition(), std::string("Unknown error (expected type)"));
	}

	return node;
}

const Node * Parser::array_type()
{
	Node * node = &Node(NodeType::array_type, word->getPosition());

	decideToken(TokenType::kw_array, std::string("expected keyword ARRAY"));
	node->addChild(*expression());
	decideToken(TokenType::kw_of, std::string("Expected keyword OF"));
	node->addChild(*type());

	return node;
}

const Node * Parser::record_type()
{
	Node * node = &Node(NodeType::record_type, word->getPosition());

	decideToken(TokenType::kw_record, std::string("expected keyword RECORD"));
	node->addChild(*field_list());

	TokenType type = scanner_->peekToken()->getType();
	while (type == TokenType::semicolon) {
		semicolon();
		node->addChild(*field_list());

		type = scanner_->peekToken()->getType();
	}
	end();

	return node;
}

const Node * Parser::field_list()
{
	Node * node = &Node(NodeType::field_list, word->getPosition());

	TokenType t_type = scanner_->peekToken()->getType();
	if (t_type == TokenType::const_ident) {
		node->addChild(*ident_list());
		double_colon();
		node->addChild(*type());
	}
	return node;
}

const Node * Parser::ident_list()
{
	Node * node = &Node(NodeType::ident_list, word->getPosition());

	node->addChild(*ident());

	while (scanner_->peekToken()->getType() != TokenType::comma) {
		decideToken(TokenType::comma, std::string("expected comma"));
		node->addChild(*ident());
	}

	return node;
}

const Node * Parser::procedure_heading()
{
	Node * node = &Node(NodeType::procedure_heading, word->getPosition());

	decideToken(TokenType::kw_procedure, std::string("expected keyword PROCEDURE"));
	node->addChild(*ident());

	if (scanner_->peekToken()->getType() == TokenType::lparen) {
		node->addChild(*formal_parameters());
	}
	return node;
}

const Node * Parser::procedure_body()
{
	Node * node = &Node(NodeType::procedure_body, word->getPosition());

	node->addChild(*declarations());

	if (scanner_->peekToken()->getType() != TokenType::kw_begin) {
		decideToken(TokenType::kw_begin, std::string("expected keyword BEGIN"));
		node->addChild(*statement_sequence());
	}
	end();
	node->addChild(*ident());
	return node;
}

const Node * Parser::formal_parameters()
{
	Node * node = &Node(NodeType::formal_parameters, word->getPosition());

	decideToken(TokenType::lparen, std::string("expected lparen"));

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::kw_var || type == TokenType::const_ident) {
		node->addChild(*fp_section());
		while (scanner_->peekToken()->getType() == TokenType::semicolon) {
			semicolon();
			node->addChild(*fp_section());
		}
	}
	decideToken(TokenType::rparen, std::string("Expected rparen"));

	return node;
}

const Node * Parser::fp_section()
{
	Node * node = &Node(NodeType::fp_section, word->getPosition());

	if (scanner_->peekToken()->getType() == TokenType::kw_var) {
		decideToken(TokenType::kw_var, std::string("expected VAR"));
	}
	node->addChild(*ident_list());
	double_colon();
	node->addChild(*type());

	return node;
}

const Node * Parser::statement_sequence()
{
	Node * node = &Node(NodeType::statement_sequence, word->getPosition());

	node->addChild(*statement());
	while (scanner_->peekToken()->getType() == TokenType::semicolon) {
		semicolon();
		node->addChild(*statement());
	}
	return node;
}

const Node * Parser::statement()
{ //TODO
	Node * node = &Node(NodeType::statement, word->getPosition());

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
const Node * Parser::A()
{
	Node * node;

	const Node * identifier = ident();
	const Node * select = selector();

	if (scanner_->peekToken()->getType() == TokenType::op_becomes) {
		node = &Node(NodeType::assignment, word->getPosition());
		node->addChild(*identifier);
		node->addChild(*select);
		decideToken(TokenType::op_becomes, std::string("Expected assignment operator \":=\""));
		node->addChild(*expression());
	}
	else {
		node = &Node(NodeType::procedure_call, word->getPosition());
		node->addChild(*identifier);
		node->addChild(*select);

		if (scanner_->peekToken()->getType() == TokenType::lparen) {
			node->addChild(*actual_parameters());
		}
	}

	return node;
}

const Node * Parser::if_statement()
{
	Node * node = &Node(NodeType::if_statement, word->getPosition());

	decideToken(TokenType::kw_if, std::string("Expected keyword IF"));
	node->addChild(*expression());
	decideToken(TokenType::kw_then, std::string("Expected keyword THEN"));
	node->addChild(*statement_sequence());

	while (scanner_->peekToken()->getType() == TokenType::kw_elsif) {
		decideToken(TokenType::kw_elsif, std::string("expected keyword ELSIF"));
		node->addChild(*expression());
		decideToken(TokenType::kw_then, std::string("Expected keyword THEN"));
		node->addChild(*statement_sequence());
	}
	if (scanner_->peekToken()->getType() == TokenType::kw_else) {
		decideToken(TokenType::kw_else, std::string("Expected keyword ELSE"));
		node->addChild(*statement_sequence());
	}
	decideToken(TokenType::kw_end, std::string("Expected keyword END"));

	return node;
}

const Node * Parser::while_statement()
{
	Node * node = &Node(NodeType::while_statement, word->getPosition());

	decideToken(TokenType::kw_while, std::string("Expected keyword WHILE"));
	node->addChild(*expression());
	decideToken(TokenType::kw_do, std::string("IEEExpected keyword DO (GROßES E"));
	node->addChild(*statement_sequence());
	decideToken(TokenType::kw_end, std::string("Expected keyword END"));
	
	return node;
}

const Node * Parser::actual_parameters()
{
	Node * node = &Node(NodeType::acutal_parameters, word->getPosition());

	decideToken(TokenType::lparen, std::string("Expected \"(\""));
	if (scanner_->peekToken()->getType() != TokenType::rparen) {
		node->addChild(*expression());
		while (scanner_->peekToken()->getType() == TokenType::comma) {
			decideToken(TokenType::comma, std::string("expected comma"));
			node->addChild(*expression());
		}
	}
	decideToken(TokenType::rparen, std::string("Expected \"(\""));

	return node;
}

const Node * Parser::selector()
{
	Node * node = &Node(NodeType::selector, word->getPosition());

	while (scanner_->peekToken()->getType() == TokenType::period
			|| scanner_->peekToken()->getType() == TokenType::lbrack) {
		if (scanner_->peekToken()->getType() == TokenType::period) {
			decideToken(TokenType::period, std::string("Expected period"));
			node->addChild(*ident());
		}
		else {
			decideToken(TokenType::lbrack, std::string("Expected \"[\""));
			node->addChild(*expression());
			decideToken(TokenType::rbrack, std::string("Expected \"]\""));
		}
	}

	return node;
}

void Parser::fail(FilePos pos, std::string &msg)
{
	logger_->error(pos, msg);
	throw new std::exception("You failed!");
}

void Parser::semicolon() {
	decideToken(TokenType::semicolon, std::string("semicolon expected"));
}

bool Parser::begin() {
	word = scanner_->nextToken();
	return word->getType() != TokenType::kw_begin;
}

void Parser::end() {
	decideToken(TokenType::kw_end, std::string("End expected"));
}

void Parser::point() {
	decideToken(TokenType::period, std::string("\".\" expected"));
}

void Parser::equals_symbol()
{
	decideToken(TokenType::op_eq, std::string("Expected assignment \"=\""));
}

void Parser::double_colon()
{
	decideToken(TokenType::colon, std::string("Expected \":\""));
}

const Node* Parser::binary_op()
{
	word = scanner_->nextToken();

	TokenType type = word->getType();
	if (type == TokenType::op_eq) {
		return &Node(NodeType::binary_op, word->getPosition(), "=");
	}
	else if (type == TokenType::op_neq) {
		return &Node(NodeType::binary_op, word->getPosition(), "#");
	}
	else if (type == TokenType::op_lt) {
		return &Node(NodeType::binary_op, word->getPosition(), "<");
	}
	else if (type == TokenType::op_leq) {
		return &Node(NodeType::binary_op, word->getPosition(), "<=");
	}
	else if (type == TokenType::op_gt) {
		return &Node(NodeType::binary_op, word->getPosition(), ">");
	}
	else if (type == TokenType::op_geq) {
		return &Node(NodeType::binary_op, word->getPosition(), ">=");
	}
	else if (type == TokenType::op_plus) {
		return &Node(NodeType::binary_op, word->getPosition(), "+");
	}
	else if (type == TokenType::op_minus) {
		return &Node(NodeType::binary_op, word->getPosition(), "-");
	}
	else if (type == TokenType::op_or) {
		return &Node(NodeType::binary_op, word->getPosition(), "OR");
	}
	else if (type == TokenType::op_times) {
		return &Node(NodeType::binary_op, word->getPosition(), "*");
	}
	else if (type == TokenType::op_div) {
		return &Node(NodeType::binary_op, word->getPosition(), "DIV");
	}
	else if (type == TokenType::op_mod) {
		return &Node(NodeType::binary_op, word->getPosition(), "MOD");
	}
	else if (type == TokenType::op_and) {
		return &Node(NodeType::binary_op, word->getPosition(), "&");
	}

	fail(word->getPosition(), std::string("Expected binary operator"));

	return nullptr;
}

void Parser::decideToken(TokenType type, std::string &errormsg)
{
	word = scanner_->nextToken();
	if (word->getType() != type) {
		fail(word->getPosition(), errormsg);
	}
}
