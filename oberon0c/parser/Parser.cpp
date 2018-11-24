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

const std::string Parser::ident()
{
	return std::string();
}

const Node * Parser::module()
{
	return nullptr;
}

const Node * Parser::declarations()
{
	return nullptr;
}

const Node * Parser::const_declarations()
{
	return nullptr;
}

const Node * Parser::type_declarations()
{
	return nullptr;
}

const Node * Parser::var_declarations()
{
	return nullptr;
}

const Node * Parser::procedure_declaration()
{
	return nullptr;
}

const Node * Parser::expression()
{
	return nullptr;
}

const Node * Parser::simple_expression()
{
	return nullptr;
}

const Node * Parser::term()
{
	return nullptr;
}

const Node * Parser::factor()
{
	return nullptr;
}

const Node * Parser::type()
{
	return nullptr;
}

const Node * Parser::array_type()
{
	return nullptr;
}

const Node * Parser::record_type()
{
	return nullptr;
}

const Node * Parser::field_list()
{
	return nullptr;
}

const Node * Parser::ident_list()
{
	return nullptr;
}

const Node * Parser::procedure_heading()
{
	return nullptr;
}

const Node * Parser::procedure_body()
{
	return nullptr;
}

const Node * Parser::formal_parameters()
{
	return nullptr;
}

const Node * Parser::fp_section()
{
	return nullptr;
}

const Node * Parser::statement_sequence()
{
	return nullptr;
}

const Node * Parser::statement()
{
	return nullptr;
}

const Node * Parser::assignment()
{
	return nullptr;
}

const Node * Parser::procedure_call()
{
	return nullptr;
}

const Node * Parser::if_statement()
{
	return nullptr;
}

const Node * Parser::while_statement()
{
	return nullptr;
}

const Node * Parser::actual_parameters()
{
	return nullptr;
}

const Node * Parser::selector()
{
	return nullptr;
}
