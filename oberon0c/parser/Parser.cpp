

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
	const Node* tree = module();

    std::unique_ptr<const Node> parse_tree(tree);
	tree = nullptr;
	   
    std::cout << *parse_tree << std::endl;

    return parse_tree;
}

const Node* Parser::ident() {
	word = scanner_->nextToken();

	if (word->getType() != TokenType::const_ident) {
        std::string s = std::string("Expected identifier. ident()");
		failToken(s);
	}
    auto* identifier = (IdentToken*) &*word;
    Node* node = new Node(NodeType::identifier, word->getPosition(), identifier->getValue(), currentTable_);
	
	return node;
}

const Node* Parser::module() {
	//Adding the first scope SymbolTable 
	std::shared_ptr<SymbolTable> newTable = std::make_shared<SymbolTable>();
	symbolTables_.push_back(newTable);
	currentTable_ = newTable;

	// Module declaration
	module_t();
	Node* moduleNode = new Node(NodeType::module, word->getPosition(), currentTable_);
	const Node* identifier = ident();
	moduleNode->addChild(identifier);
	semicolon_t();
	Symbol newDeclaration = Symbol(identifier->getValue(), std::vector<Symbol*>(), SymbolType::module, false);
	if (currentTable_->insert(newDeclaration)) {
		failSymbolExists(&newDeclaration);
	}

	// Declarations
	moduleNode->addChild(declarations());

	// Optional: Begin (main method)
	if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
		begin_t();
		moduleNode->addChild(statement_sequence());
	}

	//End
	end_t();
	if (identifier->getValue() != ident()->getValue()) {
		std::string s = std::string("Expected module identifier module()");
		failToken(s);
	}
	point_t();

	currentTable_ = nullptr;
	return moduleNode;
}

const Node* Parser::declarations() {
	Node* node = new Node(NodeType::declarations, word->getPosition(), currentTable_);

	// CONSTs
	if (scanner_->peekToken()->getType() == TokenType::kw_const) {
		const_t();

		while (scanner_->peekToken()->getType() == TokenType::const_ident) {
			node->addChild(const_declarations());
		}
	}

	// TYPEs
	if (scanner_->peekToken()->getType() == TokenType::kw_type) {
		type_t();
		while (scanner_->peekToken()->getType() == TokenType::const_ident) {
			node->addChild(type_declarations());
		}
	}

	// VARs
	if (scanner_->peekToken()->getType() == TokenType::kw_var) {
		var_t();
		while (scanner_->peekToken()->getType() == TokenType::const_ident) {
			node->addChild(var_declarations());
		}
	}


	// Optional Procedures
	while (scanner_->peekToken()->getType() == TokenType::kw_procedure) {
		node->addChild(procedure_declaration());
	}

	return node;
}

const Node* Parser::const_declarations() {
	Node* node = new Node(NodeType::const_declarations, word->getPosition(), currentTable_);

	// Processing one constant
	const Node * identifier = ident();
	node->addChild(identifier);
	equals_symbol_t();
	node->addChild(expression());
	semicolon_t();

	// Add processed constant to the symbol table
	Symbol * symbol = currentTable_->getSymbol(&std::string("INTEGER"));
	std::vector<Symbol*> types;
	types.push_back(symbol);
	Symbol newConst = Symbol(identifier->getValue(), types, SymbolType::constant, true);
	if (currentTable_->insert(newConst)) {
		failSymbolExists(&newConst);
	}

	return node;
}

const Node* Parser::type_declarations() {
	Node* node = new Node(NodeType::type_declarations, word->getPosition(), currentTable_);

	// Processing one type
	const Node * identifier = ident();
	node->addChild(identifier);
	equals_symbol_t();
	const Node * typeNode = type();
	node->addChild(typeNode);
	semicolon_t();

	// Add processed type to the symbol table
	const Node* typeDef = typeNode->getChildren().at(0);
	switch (typeDef->getNodeType()) {
		case (NodeType::identifier): {
			addType(identifier, typeDef, false);
		}
			break;
		case (NodeType::array_type): {
			addArray(identifier, typeDef, false);
		}
			break;
		case (NodeType::record_type): {
			addRecord(node, identifier, typeDef, false);
		}
			break;
	}

	return node;
}

const Node* Parser::var_declarations() {
	Node* node = new Node(NodeType::var_declarations, word->getPosition(), currentTable_);

	const Node* identifiersNode = ident_list();
	node->addChild(identifiersNode);
	double_colon_t();
	const Node* typeNode = type();
	node->addChild(typeNode);
	semicolon_t();

	std::vector<const Node *> identifiers = identifiersNode->getChildren();
	const Node* typeDef = typeNode->getChildren().at(0);
	switch (typeDef->getNodeType()) {
		case (NodeType::identifier): {
			for (const Node* identifier : identifiers) {
				switch (currentTable_->getSymbol(&typeDef->getValue())->getSymbolType()) {
				case SymbolType::array:
					addArray(identifier, typeDef, true);
					break;
				case SymbolType::record: {
					addRecord(node, identifier, typeDef, true);
					Symbol* identifierSymbol = currentTable_->getSymbol(&identifier->getValue());
					Symbol* typeDefSymbol = currentTable_->getSymbol(&typeDef->getValue());
					recordsSymbolTables_[identifierSymbol] = recordsSymbolTables_[typeDefSymbol];
					break;
				}
				case SymbolType::type:
					addType(identifier, typeDef, true);
					break;
				}
			}
		}
			break;
		case (NodeType::array_type): {
			for (const Node* identifier : identifiers) {
				addArray(identifier, typeDef, true);
			}
		}
			break;
		case (NodeType::record_type): {
			for (const Node* identifier : identifiers) {
				addRecord(node, identifier, typeDef, true);
			}
		}
			break;
	}

	return node;
}

const Node* Parser::procedure_declaration() {
	Node* node = new Node(NodeType::procedure_declaration, word->getPosition(), currentTable_);

	node->addChild(procedure_heading());
	semicolon_t();
	node->addChild(procedure_body());

	// TODO currentTable_ to parent
	currentTable_ = node->getSymbolTable();

	return node;
}

const Node* Parser::expression() {
	Node* node = new Node(NodeType::expression, word->getPosition(), currentTable_);

	node->addChild(simple_expression());

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::op_eq || type == TokenType::op_neq
		|| type == TokenType::op_lt || type == TokenType::op_leq
		|| type == TokenType::op_gt || type == TokenType::op_geq) {
		node->addChild(binary_op());
		node->addChild(simple_expression());
	}

	return node;
}

const Node* Parser::simple_expression() {
	Node* node = new Node(NodeType::simple_expression, word->getPosition(), currentTable_);

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::op_plus || type == TokenType::op_minus) {
		node->addChild(binary_op());
	}
	node->addChild(term());

	type = scanner_->peekToken()->getType();
	while (type == TokenType::op_plus || type == TokenType::op_minus
		|| type == TokenType::op_or) {
		node->addChild(binary_op());
		node->addChild(term());

		type = scanner_->peekToken()->getType();
	}


	return node;
}

const Node* Parser::term() {
	Node* node = new Node(NodeType::term, word->getPosition(), currentTable_);

	node->addChild(factor());

	TokenType type = scanner_->peekToken()->getType();
	while (type == TokenType::op_times || type == TokenType::op_div
		|| type == TokenType::op_mod || type == TokenType::op_and) {
		node->addChild(binary_op());
		node->addChild(factor());

		type = scanner_->peekToken()->getType();
	}

	return node;
}

const Node* Parser::number() {
    word = scanner_->nextToken();

    if (word->getType() != TokenType::const_number) {
        std::string s = std::string("Expected number.");
        failToken(s);
    }
    auto* number = (NumberToken*) &*word;
    Node* node = new Node(NodeType::number, word->getPosition(), std::to_string(number->getValue()), currentTable_);

    return node;
}


const Node* Parser::factor() {
	Node* node = new Node(NodeType::factor, word->getPosition(), currentTable_);


	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		// Check if the identifier is declared and appropriate (a record or an array).
		const Node* identifier = ident();
		failUndeclaredSymbol(identifier);
		failIfNotAVariable(identifier);
		node->addChild(identifier);

		switch (scanner_->peekToken()->getType()) {
			case TokenType::period:
				failIfNotARecord(identifier);
				break;
			case TokenType::lbrack:
				failIfNotAArray(identifier);
				break;
			case TokenType::const_number:
			case TokenType::lparen:
			case TokenType::op_not:
			case TokenType::kw_of: 
			case TokenType::op_eq:
			case TokenType::op_neq:
			case TokenType::op_lt:
			case TokenType::op_leq:
			case TokenType::op_gt:
			case TokenType::op_geq:
			case TokenType::kw_do:
			case TokenType::op_minus:
			case TokenType::op_plus:
			case TokenType::rbrack:
			case TokenType::semicolon:
			case TokenType::kw_end: {
				Symbol* symbol = currentTable_->getSymbol(&identifier->getValue());
				SymbolType symType = symbol->getSymbolType();
				if (symType != SymbolType::constant
					&& symType != SymbolType::type) {
					std::string msg = std::string(identifier->getValue() + " is not an appropriate type");
					failSymbol(msg);
				}
			}
				break;
			default:
				failNetiherRecordNorArray(identifier); //TODO: Check if this makes sense since this could also be a procedure?!
		}

		node->addChilds(selector(identifier));
	}
	else if (type == TokenType::const_number) {
        node->addChild(number());
	}
	else if (type == TokenType::lparen) {
		lparen_t();
		node->addChild(expression());
		rparen_t();
	}
	else if (type == TokenType::op_not) {
		not_t();
		node->addChild(factor());
	}
	else {
		word = scanner_->nextToken();
		std::string s = std::string("Expected a factor but was something else factor()");
		failToken(s);
	}

	return node;
}

const Node* Parser::type() {
	Node* node = new Node(NodeType::type, word->getPosition(), currentTable_);

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		node->addChild(ident());
	}
	else if (type == TokenType::kw_array) {
		node->addChild(array_type());
	}
	else if (type == TokenType::kw_record) {
		node->addChild(record_type());
	}
	else {
	    std::string s = std::string("Unknown error (expected type) type()");
		failToken(s);
	}

	return node;
}

const Node* Parser::array_type() {
	Node* node = new Node(NodeType::array_type, word->getPosition(), currentTable_);

	array_t();
	node->addChild(expression());
	of_t();
	node->addChild(type());

	return node;
}

const Node* Parser::record_type() {
	Node* node = new Node(NodeType::record_type, word->getPosition(), currentTable_);

	record_t();
	node->addChild(field_list());

	TokenType type = scanner_->peekToken()->getType();
	while (type == TokenType::semicolon) {
		semicolon_t();
		node->addChild(field_list());

		type = scanner_->peekToken()->getType();
	}
	end_t();

	return node;
}

const Node* Parser::field_list() {
	Node* node = new Node(NodeType::field_list, word->getPosition(), currentTable_);

	TokenType t_type = scanner_->peekToken()->getType();
	if (t_type == TokenType::const_ident) {
		node->addChild(ident_list());
		double_colon_t();
		node->addChild(type());
	}
	return node;
}

const Node* Parser::ident_list() {
	Node* node = new Node(NodeType::ident_list, word->getPosition(), currentTable_);

	node->addChild(ident());

	while (scanner_->peekToken()->getType() == TokenType::comma) {
		comma_t();
		node->addChild(ident());
	}

	return node;
}

const Node* Parser::procedure_heading() {
	Node* node = new Node(NodeType::procedure_heading, word->getPosition(), currentTable_);

	// Parse procedure identifier
	procedure_t();
	const Node* procIdent = ident();
	node->addChild(procIdent);

	// Add symbol for procedure.
	Symbol procSymbol = Symbol(procIdent->getValue(), std::vector<Symbol*>(), SymbolType::procedure, true);
	if (currentTable_->insert(procSymbol)) {
		failSymbolExists(&procSymbol);
	}

	// Creating symbolTable for the new lexicalScope in the new procedure.
	newSymbolTable();

	// Parse formal parameters
	if (scanner_->peekToken()->getType() == TokenType::lparen) {
		// Parsing
		const Node* fParams = formal_parameters();
		node->addChild(fParams);

		// Adding identifiers of procedure parameters
		for (const Node* fpSection : fParams->getChildren()) {
			std::vector<const Node*> children = fpSection->getChildren();
			size_t identPosition = 0;// children.at(0).getNodeType() != NodeType::var_declarations;
			const Node* varIdentifiers = children.at(identPosition);
			const Node* typeDef = children.at(++identPosition)->getChildren().at(0);

			switch (typeDef->getNodeType()) {
			case (NodeType::identifier): {
				for (const Node* identifier : varIdentifiers->getChildren()) {
					addType(identifier, typeDef, true);
				}
			}
					break;
			case (NodeType::array_type): {
				for (const Node* identifier : varIdentifiers->getChildren()) {
					addArray(identifier, typeDef, true);
				}
			}
				break;
			case (NodeType::record_type): {
				for (const Node* identifier : varIdentifiers->getChildren()) {
					addRecord(node, identifier, typeDef, true);
				}
			}
				break;
			}
		}
	}

	return node;
}

const Node* Parser::procedure_body()
{
	Node* node = new Node(NodeType::procedure_body, word->getPosition(), currentTable_);

	node->addChild(declarations());

	if (scanner_->peekToken()->getType() == TokenType::kw_begin) {
		begin_t();
		node->addChild(statement_sequence());
	}
	end_t();
	node->addChild(ident());
	semicolon_t();

	return node;
}

const Node* Parser::formal_parameters()
{
	Node* node = new Node(NodeType::formal_parameters, word->getPosition(), currentTable_);

	lparen_t();

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::kw_var || type == TokenType::const_ident) {
		node->addChild(fp_section());
		while (scanner_->peekToken()->getType() == TokenType::semicolon) {
			semicolon_t();
			node->addChild(fp_section());
		}
	}
	rparen_t();

	return node;
}

const Node* Parser::fp_section()
{
	Node* node = new Node(NodeType::fp_section, word->getPosition(), currentTable_);

	if (scanner_->peekToken()->getType() == TokenType::kw_var) {
		var_t();
	}
	node->addChild(ident_list());
	double_colon_t();
	node->addChild(type());

	return node;
}

const Node* Parser::statement_sequence()
{
	Node* node = new Node(NodeType::statement_sequence, word->getPosition(), currentTable_);

	node->addChild(statement());
	while (scanner_->peekToken()->getType() == TokenType::semicolon) {
		semicolon_t();
		node->addChild(statement());
	}
	return node;
}

const Node* Parser::statement()
{ //TODO
	Node* node = new Node(NodeType::statement, word->getPosition(), currentTable_);

	TokenType type = scanner_->peekToken()->getType();
	if (type == TokenType::const_ident) {
		node->addChild(A());
	}
	else if (type == TokenType::kw_if) {
		node->addChild(if_statement());
	}
	else if (type == TokenType::kw_while) {
		node->addChild(while_statement());
	}


	return node;

}

// Implements Assignment & ProcedureCall
const Node* Parser::A()
{
	Node* node;

	// Get the identifier of this assignment or procedure call
	const Node* identifier = ident();
	failUndeclaredSymbol(identifier);
	// Peek ahead the right next token before processing the selector.
	const Token followIdentifier = *scanner_->peekToken();
	
	std::shared_ptr<std::vector<const Node*>> select = selector(identifier);

	const Token* peeked = scanner_->peekToken();
	if (peeked->getType() == TokenType::op_becomes) {
		failIfNotAVariable(identifier);
		node = new Node(NodeType::assignment, word->getPosition(), currentTable_);
		node->addChild(identifier);
		// Check the previously peeked token to decide if the identifier is used as record or array
		// and check if the identifier correspondingly is a record or a array.
		switch (followIdentifier.getType()) {
		case TokenType::period:
			failIfNotARecord(identifier);
			node->addChilds(select);
			break;
		case TokenType::lbrack:
			failIfNotAArray(identifier);
			node->addChilds(select);
			break;
		case TokenType::op_becomes: {
			Symbol* symbol = currentTable_->getSymbol(&identifier->getValue());
			if (symbol->getSymbolType() != SymbolType::constant
				&& symbol->getSymbolType() != SymbolType::type) {
				std::string msg = std::string(identifier->getValue() + " is not a appropriate type");
				failSymbol(msg);
			}
		}
			break;
		default:
			failNetiherRecordNorArray(identifier);
		}
		becomes_t();
		node->addChild(expression());
	}
	else {
		// Procedure could be the identifier or (if present) the last selector behind the identifier.
		// Note: Actually in the latter case we'd have to check if the procedure hangs behind modules but as we do not have a linker
		// we cannot do that.
		const Node* procedure = (select->size() > 0) ? select->back()->getChildren().at(0) : identifier;
		failIfNotProcedure(procedure);

		node = new Node(NodeType::procedure_call, word->getPosition(), currentTable_);
		node->addChild(identifier);
		node->addChilds(select);

		if (scanner_->peekToken()->getType() == TokenType::lparen) {
			node->addChild(actual_parameters());
		}
	}

	return node;
}

const Node* Parser::if_statement()
{
	Node* node = new Node(NodeType::if_statement, word->getPosition(), currentTable_);

	if_t();
	node->addChild(expression());
	then_t();
	node->addChild(statement_sequence());

	while (scanner_->peekToken()->getType() == TokenType::kw_elsif) {
		elseif_t();
		node->addChild(expression());
		then_t();
		node->addChild(statement_sequence());
	}
	if (scanner_->peekToken()->getType() == TokenType::kw_else) {
		else_t();
		node->addChild(statement_sequence());
	}
	end_t();

	return node;
}

const Node* Parser::while_statement()
{
	Node* node = new Node(NodeType::while_statement, word->getPosition(), currentTable_);

	while_t();
	node->addChild(expression());
	do_t();
	node->addChild(statement_sequence());
	end_t();

	return node;
}

const Node* Parser::actual_parameters()
{
	Node* node = new Node(NodeType::acutal_parameters, word->getPosition(), currentTable_);

	lparen_t();
	if (scanner_->peekToken()->getType() != TokenType::rparen) {
		node->addChild(expression());
		while (scanner_->peekToken()->getType() == TokenType::comma) {
			comma_t();
			node->addChild(expression());
		}
	}
	rparen_t();

	return node;
}

const std::shared_ptr<std::vector<const Node*>> Parser::selector(const Node * preceedingIdentifier) {
	auto selectors = std::make_shared<std::vector<const Node*>>();
	std::vector<TokenType>* tokenTypes = &std::vector<TokenType>();

	TokenType peekTokenType = scanner_->peekToken()->getType();
	while (peekTokenType == TokenType::period
		|| peekTokenType == TokenType::lbrack) {
		// Get select and add it to return values
		const Node* select = B(preceedingIdentifier);
		selectors->push_back(select);
		tokenTypes->push_back(peekTokenType);

		peekTokenType = scanner_->peekToken()->getType();
		/*std::vector<const Node*> selectChildren = select->getChildren();

		// Check if the selector is again a selectable identifier and if so make it the next preceeding symbol.
		// Check also if the next preceeding identifier is of an appropriate type for the next symbol.
		if (selectChildren.size() > 0) {
			const Node* preceedingCandidate = selectChildren.at(0);
			if (preceedingCandidate->getNodeType() == NodeType::identifier) {
				Symbol* currentPreceedingSymbol = currentTable_->getSymbol(&preceedingIdentifier->getValue());
				std::shared_ptr<SymbolTable> nextPreceedingSymbolTable = recordsSymbolTables_[currentPreceedingSymbol];
				Symbol* nextPreceedingSymbol = nextPreceedingSymbolTable->getSymbol(&preceedingCandidate->getValue());

				switch (scanner_->peekToken()->getType()) {
				case TokenType::period:
					failIfNotARecord(preceedingCandidate);
					break;
				case TokenType::lbrack:
					failIfNotAArray(preceedingCandidate);
					break;
				}
				preceedingIdentifier = preceedingCandidate;
			}
		}*/
	}

	for (int i = 0; i < (int)(selectors->size()) - 1; i++) {
		TokenType tokenType = (*tokenTypes)[i+1];
		const Node* select = (*selectors)[i];
		std::vector<const Node*> selectChildren = select->getChildren();

		if (selectChildren.size() > 0) {
			const Node* preceedingCandidate = selectChildren.at(0);

			Symbol* preceedingSymbol = currentTable_->getSymbol(&preceedingIdentifier->getValue());
			std::shared_ptr<SymbolTable> recordsTable = recordsSymbolTables_[preceedingSymbol];
			Symbol* candidateSymbol = recordsTable->getSymbol(&preceedingCandidate->getValue());

			failUndeclaredSymbol(candidateSymbol, preceedingCandidate);

			switch (tokenType) {
			case TokenType::period:
				// Next after candidate is period, so the candidate must be a record.
				failIfNotARecord(preceedingCandidate, recordsTable);
				break;
			case TokenType::lbrack:
				// Next after candidate is expression, so this must be an array
				failIfNotAArray(preceedingCandidate, recordsTable);
				break;
			}
		}
	}

	return selectors;
}

const Node* Parser::B(const Node* preceedingIdentifier) {
	Node* node = new Node(NodeType::selector, word->getPosition(), currentTable_);

	if (scanner_->peekToken()->getType() == TokenType::period) {
		point_t();

		// Find the receferenced identifier in the records inner symbol table
		const Node* identifier = ident();
		Symbol* preceedingSymbol = currentTable_->getSymbol(&preceedingIdentifier->getValue());
		//Symbol* preceedingSymbolType = preceedingSymbol->getTypes()->at(0);

		std::shared_ptr<SymbolTable> recordsIdentifiers = recordsSymbolTables_[preceedingSymbol];
		Symbol* identifiersSymbol = recordsIdentifiers->getSymbol(&identifier->getValue());
		failUndeclaredSymbol(identifiersSymbol, identifier);
		failIfNotAVariable(identifiersSymbol);

		node->addChild(identifier);
	}
	else {
		lbrack_t();
		node->addChild(expression());
		rbrack_t();
	}


	return node;
}

const Node* Parser::binary_op() {
	word = scanner_->nextToken();

	TokenType type = word->getType();
	if (type == TokenType::op_eq) {
		return new Node(NodeType::binary_op, word->getPosition(), "=", currentTable_);
	}
	else if (type == TokenType::op_neq) {
		return new Node(NodeType::binary_op, word->getPosition(), "#", currentTable_);
	}
	else if (type == TokenType::op_lt) {
		return new Node(NodeType::binary_op, word->getPosition(), "<", currentTable_);
	}
	else if (type == TokenType::op_leq) {
		return new Node(NodeType::binary_op, word->getPosition(), "<=", currentTable_);
	}
	else if (type == TokenType::op_gt) {
		return new Node(NodeType::binary_op, word->getPosition(), ">", currentTable_);
	}
	else if (type == TokenType::op_geq) {
		return new Node(NodeType::binary_op, word->getPosition(), ">=", currentTable_);
	}
	else if (type == TokenType::op_plus) {
		return new Node(NodeType::binary_op, word->getPosition(), "+", currentTable_);
	}
	else if (type == TokenType::op_minus) {
		return new Node(NodeType::binary_op, word->getPosition(), "-", currentTable_);
	}
	else if (type == TokenType::op_or) {
		return new Node(NodeType::binary_op, word->getPosition(), "OR", currentTable_);
	}
	else if (type == TokenType::op_times) {
		return new Node(NodeType::binary_op, word->getPosition(), "*", currentTable_);
	}
	else if (type == TokenType::op_div) {
		return new Node(NodeType::binary_op, word->getPosition(), "DIV", currentTable_);
	}
	else if (type == TokenType::op_mod) {
		return new Node(NodeType::binary_op, word->getPosition(), "MOD", currentTable_);
	}
	else if (type == TokenType::op_and) {
		return new Node(NodeType::binary_op, word->getPosition(), "&", currentTable_);
	}

	std::string s = std::string("Expected binary operator in method binary op");
	failToken(s);

	return nullptr;
}

void Parser::failToken(std::string &msg) {
    std::stringstream ss;
    ss << "\"" << *word << "\"" << std::endl;
    msg = msg + " expected but got " + ss.str();

	logger_->error(word->getPosition(), msg);
	throw  std::invalid_argument("You failed!" + msg);
}

void Parser::failSymbol(std::string &msg) {
	logger_->error(word->getPosition(), msg);
	throw std::invalid_argument("You failed!" + msg);
}

void Parser::failUndeclaredSymbol(Symbol * undeclaredSymbol, const Node * identifier)
{
	if (undeclaredSymbol != nullptr) {
		return;
	}
	std::stringstream ss;
	ss << identifier->getValue() << " was not declared.";
	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!" + ss.str());
}

void Parser::failUndeclaredSymbol(const Node * identifier)
{
	Symbol* undeclaredSymbol = currentTable_->getSymbol(&identifier->getValue());
	failUndeclaredSymbol(undeclaredSymbol, identifier);
}

void Parser::failIfNotAType(Symbol *identifier) {
	const SymbolType sType = identifier->getSymbolType();
	if (sType == SymbolType::module
		|| sType == SymbolType::procedure
		|| sType == SymbolType::constant) {
		std::stringstream ss;
		ss << identifier->getName() << " is not a type.";
		logger_->error(word->getPosition(), ss.str());
		throw std::invalid_argument("You failed!" + ss.str());
	}
}

void Parser::failSymbolExists(Symbol * symbol)
{
	std::stringstream ss;
	ss << *symbol->getName() << " already exists";
	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!" + ss.str());
}

void Parser::failIfNotASomething(const Node * identifier, SymbolType symbolType)
{
	failIfNotASomething(identifier, symbolType, currentTable_);
}

void Parser::failIfNotASomething(const Node * identifier, SymbolType symbolType, std::shared_ptr<SymbolTable> symbolTable)
{
	Symbol* possibleRecord = symbolTable->getSymbol(&identifier->getValue());
	if (possibleRecord->getSymbolType() == symbolType) {
		return;
	}

	std::stringstream ss;
	ss << *possibleRecord->getName() << " is not ";
	switch (symbolType) {
	case SymbolType::array:
		ss << "an Array";
		break;
	case SymbolType::record:
		ss << "a Record";
		break;
	case SymbolType::type:
		ss << "a Type";
		break;
	}
	ss << ".";

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!" + ss.str());
}

void Parser::failIfNotARecord(const Node * identifier)
{
	failIfNotASomething(identifier, SymbolType::record);
}

void Parser::failIfNotARecord(const Node * identifier, std::shared_ptr<SymbolTable> symbolTable)
{
	failIfNotASomething(identifier, SymbolType::record, symbolTable);
}

void Parser::failIfNotAArray(const Node * identifier)
{
	failIfNotASomething(identifier, SymbolType::array);
}

void Parser::failIfNotAArray(const Node * identifier, std::shared_ptr<SymbolTable> symbolTable)
{
	failIfNotASomething(identifier, SymbolType::array, symbolTable);
}

void Parser::failNetiherRecordNorArray(const Node * identifier)
{
	std::stringstream ss;
	ss << identifier->getValue() << " is neither an Array nor a Record";
	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!" + ss.str());
}

void Parser::failIfNotProcedure(const Node * identifier)
{
	if (currentTable_->getSymbol(&identifier->getValue())->getSymbolType() == SymbolType::procedure) {
		return;
	}

	std::stringstream ss;
	ss << identifier->getValue() << " is not a procedure";
	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!" + ss.str());
}

void Parser::failIfNotAVariable(Symbol * variable)
{
	if (variable->isVariable()) {
		return;
	}

	std::stringstream ss;
	ss << *variable->getName() << " is a type";
	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!" + ss.str());
}

void Parser::failIfNotAVariable(const Node * identifier)
{
	Symbol* symbol = currentTable_->getSymbol(&identifier->getValue());
	failIfNotAVariable(symbol);
}

void Parser::newSymbolTable()
{
	std::shared_ptr<SymbolTable> newTable = currentTable_->nestedTable(currentTable_);
	symbolTables_.push_back(newTable);
	currentTable_ = newTable;
}

void Parser::addType(const Node * identifier, const Node * typeDef, bool asVariable)
{		
	// New type is an "alias" of an existing type. Check if that existing type exists
	Symbol* type = currentTable_->getSymbol(&typeDef->getValue());
	failUndeclaredSymbol(type, typeDef);
	// The type exists. Add the alias of that type to the symbol table.
	std::vector<Symbol*> types;
	types.push_back(type);
	Symbol newAlias = Symbol(identifier->getValue(), types, SymbolType::type, asVariable);
	if (currentTable_->insert(newAlias)) {
		failSymbolExists(&newAlias);
	}
}

void Parser::addArray(const Node * identifier, const Node * typeDef, bool asVariable)
{
	// New type is an array. Check if the specified array type exists.
	const Node* typeDef2 = typeDef->getChildren().at(1)->getChildren().at(0);
	Symbol* type = currentTable_->getSymbol(&typeDef2->getValue());
	failUndeclaredSymbol(type, typeDef2);
	failIfNotAType(type);

	// The type exists. Add the array of that type to the symbol table.
	std::vector<Symbol*> types;
	types.push_back(type);
	Symbol newArray = Symbol(identifier->getValue(), types, SymbolType::array, asVariable);
	if (currentTable_->insert(newArray)) {
		failSymbolExists(&newArray);
	}
}

void Parser::addRecord(Node* node, const Node * identifier, const Node * typeDef, bool asVariable)
{
	// New lexical scope
	newSymbolTable();

	// New Type is a record. Check if all specified types exist.
	std::vector<Symbol*> recordTypes;
	const std::vector<const Node*> fieldLists = typeDef->getChildren();
	for (const Node* fieldList : fieldLists) {
		const Node* identifierList = fieldList->getChildren().at(0);
		const Node* typeNode2 = fieldList->getChildren().at(1);
		const Node* typeIdentifier = typeNode2->getChildren().at(0);
		Symbol* type = currentTable_->getSymbol(&typeIdentifier->getValue());
		std::vector<Symbol*> types;
		types.push_back(type);
		failUndeclaredSymbol(type, typeIdentifier);
		failIfNotAType(type);

		// Type exists. Add the identifiers.
		std::vector<const Node*> identifiers = fieldList->getChildren().at(0)->getChildren();
		for (const Node* ident : identifiers) {
			Symbol newIdent(ident->getValue(), types, SymbolType::type, true);
			if (currentTable_->insert(newIdent)) {
				failSymbolExists(&newIdent);
			}
			recordTypes.push_back(type);
		}
	}

	// Finished adding symbols to lexical subscope. Adding RecordType to current scope.
	std::shared_ptr<SymbolTable> recordsSymbolTable = currentTable_;
	currentTable_ = node->getSymbolTable();
	Symbol newRecord = Symbol(identifier->getValue(), recordTypes, SymbolType::record, asVariable);
	if (currentTable_->insert(newRecord)) {
		failSymbolExists(&newRecord);
	}

	// Add the records symbol table to the map.
	Symbol* recordsSymbol = currentTable_->getSymbol(newRecord.getName());
	recordsSymbolTables_.insert(std::unordered_map<Symbol*, std::shared_ptr<SymbolTable>>::value_type(recordsSymbol, recordsSymbolTable));
}

void Parser::postParserTypeCheck(const Node * module)
{
	switch (module->getNodeType()) {
	case (NodeType::module):
		break;
	case (NodeType::declarations):
		break;
	case (NodeType::const_declarations):
		// Check that all referenced identifiers evaluate to integers or constants.
		break;
	case (NodeType::type_declarations):
		break;
	case (NodeType::var_declarations):
		break;
	case (NodeType::procedure_declaration):
		break;
	case (NodeType::identifier):
		break;
	case (NodeType::number):
		break;
	case (NodeType::binary_op):
		break;
	case (NodeType::expression):
		break;
	case (NodeType::simple_expression):
		break;
	case (NodeType::term):
		break;
	case (NodeType::factor):
		break;
	case (NodeType::type):
		break;
	case (NodeType::record_type):
		break;
	case (NodeType::array_type):
		break;
	case (NodeType::field_list):
		break;
	case (NodeType::ident_list):
		break;
	case (NodeType::procedure_heading):
		break;
	case (NodeType::procedure_body):
		break;
	case (NodeType::formal_parameters):
		break;
	case (NodeType::fp_section):
		break;
	case (NodeType::statement_sequence):
		break;
	case (NodeType::statement):
		break;
	case (NodeType::if_statement):
		break;
	case (NodeType::while_statement):
		break;
	case (NodeType::acutal_parameters):
		break;
	case (NodeType::selector):
		break;
	case (NodeType::assignment):
		break;
	case (NodeType::procedure_call):
		break;
	}
}

Symbol * Parser::typeOfSimpleExpression(const Node * simpleExpression)
{
	return nullptr;
}

Symbol * Parser::typeOfExpression(const Node * expression)
{
	return nullptr;
}

Symbol * Parser::typeOfTerm(const Node * term)
{
	return nullptr;
}

Symbol * Parser::typeOfFactor(const Node * factor)
{
	return nullptr;
}

Symbol * Parser::typeOfSelector(const Node * selector)
{
	return nullptr;
}

Symbol * Parser::typeOfIdentifier(const Node * identifier)
{
	std::shared_ptr<SymbolTable> st = identifier->getSymbolTable();
	Symbol* symbol = st->getSymbol(&identifier->getValue());
	std::vector<Symbol*>* types = symbol->getTypes();
	if (types->size() > 1) {
		// It's a record. Irrelevant to return a type.
		return nullptr;
	}
	else if (types->size() == 0) {
		throw std::invalid_argument("Tried to infer a type of something that is not a type");
	}
	else {
		return types->at(0);
	}
}

void Parser::module_t()
{
	std::string s = std::string("\"MODULE\"");
	decideToken(TokenType::kw_module, s);
}

void Parser::semicolon_t() {
    std:: string s = std::string("semicolon");
	decideToken(TokenType::semicolon, s);
}

void Parser::comma_t() {
    std::string s = std::string("comma");
	decideToken(TokenType::comma, s);
}

void Parser::begin_t() {
    std::string s = std::string("\"BEGIN\"");
	decideToken(TokenType::kw_begin, s);
}

void Parser::end_t() {
    std::string s =  std::string("\"END\"");
	decideToken(TokenType::kw_end, s);
}

void Parser::point_t() {
    std::string s = std::string("\".\"");
	decideToken(TokenType::period, s);
}

void Parser::equals_symbol_t() {
    std::string s = std::string("\"=\"");
	decideToken(TokenType::op_eq, s);
}

void Parser::double_colon_t() {
    std::string s = std::string("\":\"");
	decideToken(TokenType::colon, s);
}

void Parser::lparen_t() {
    std::string s = std::string("\"(\"");
	decideToken(TokenType::lparen, s);
}

void Parser::rparen_t() {
    std::string s = std::string("\")\"");
	decideToken(TokenType::rparen, s);
}

void Parser::lbrack_t() {
    std::string s = std::string("\"[\"");
	decideToken(TokenType::lbrack, s);
}

void Parser::rbrack_t() {
    std::string s = std::string("\"]\"");
	decideToken(TokenType::rbrack, s);
}

void Parser::not_t() {
    std::string s = std::string("\"]\"");
	decideToken(TokenType::op_not, s);
}

void Parser::array_t() {
    std::string s = std::string("\"ARRAY\"");
	decideToken(TokenType::kw_array, s);
}

void Parser::of_t() {
    std::string s = std::string("\"OF\"");
	decideToken(TokenType::kw_of, s);
}

void Parser::record_t() {
    std::string s = std::string("\"RECORD\"");
	decideToken(TokenType::kw_record, s);
}

void Parser::procedure_t() {
    std::string s = std::string("\"PROCEDURE\"");
	decideToken(TokenType::kw_procedure, s);
}

void Parser::const_t() {
    std::string s = std::string("\"CONST\"");
	decideToken(TokenType::kw_const, s);
}

void Parser::type_t() {
    std::string s = std::string("\"TYPE\"");
	decideToken(TokenType::kw_type, s);
}

void Parser::var_t() {
    std::string s = std::string("\"VAR\"");
	decideToken(TokenType::kw_var, s);
}

void Parser::becomes_t() {
    std::string s = std::string("\":=\"");
	decideToken(TokenType::op_becomes, s);
}

void Parser::if_t() {
    std::string s = std::string("\"IF\"");
	decideToken(TokenType::kw_if, s);
}

void Parser::then_t() {
    std::string s = std::string("\"THEN\"");
	decideToken(TokenType::kw_then, s);
}

void Parser::else_t() {
    std::string s = std::string("\"ELSE\"");
	decideToken(TokenType::kw_else, s);
}

void Parser::elseif_t() {
    std::string s = std::string("\"ELSEIF\"");
	decideToken(TokenType::kw_elsif, s);
}

void Parser::while_t() {
    std::string s = std::string("\"WHILE\"");
	decideToken(TokenType::kw_while, s);
}

void Parser::do_t() {
    std::string s =  std::string("\"DO\"");
	decideToken(TokenType::kw_do, s);
}

void Parser::decideToken(TokenType type, std::string &errormsg) {
	word = scanner_->nextToken();
	if (word->getType() != type) {
		failToken(errormsg);
	}
}
