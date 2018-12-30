

//
// Created by Michael Grossniklaus on 11/20/18.
//

#include <iostream>
#include <stdexcept>
#include <IdentToken.h>
#include <NumberToken.h>
#include <utility>
#include "ast/BuildAST.h"
#include "Parser.h"

Parser::Parser(Scanner *scanner, Logger *logger) :
	scanner_(scanner), logger_(logger) {
}

Parser::~Parser() = default;

const std::unique_ptr<Node> Parser::parse() {
	// Parse the source file
	std::shared_ptr<Node> tree = module();
	// Post parser typechecking (additional to in-parser-typechecking)
	postParserTypeCheck(moduleNode.get(), std::string());
	// Building abstract syntax trees
	std::unique_ptr<BuildAST> ast = std::make_unique<BuildAST>(currentTable_, tree);
	ast->build();

	// Making the parse tree a unique pointer and throwing away the old one.
    std::unique_ptr<Node> parse_tree(tree.get());
	tree = nullptr;
	   
	// Print the parse tree.
    std::cout << *parse_tree << std::endl;
	// Print the symbol tables.
	std::cout << *symbolTables_.front() << std::endl;
	// Print the AST(s)
	auto mainSymbolTable = parse_tree->getSymbolTable();
	auto map = mainSymbolTable->getMap();
	for (auto item : map) {
		if (item.second.getSymbolType() == SymbolType::module
			|| item.second.getSymbolType() == SymbolType::procedure) {
			std::cout << *item.second.getName() << std::endl;
			std::cout << *item.second.getAst() << std::endl;
		}
	}

    return parse_tree;
}

const Node* Parser::ident() {
	word = scanner_->nextToken();

	if (word->getType() != TokenType::const_ident) {
        std::string s = std::string("Expected identifier. ident()");
		failToken(s);
	}
    auto* identifier = (IdentToken*) &*word;
    const Node* node = new Node(NodeType::identifier, word->getPosition(), identifier->getValue(), currentTable_);
	
	return node;
}

const std::shared_ptr<Node> Parser::module() {

	// Module declaration
	module_t();
	const Node* identifier = ident();

	//Adding the first scope SymbolTable 
	std::shared_ptr<SymbolTable> newTable = std::make_shared<SymbolTable>(identifier->getValue());
	symbolTables_.push_back(newTable);
	currentTable_ = newTable;

	//const Node* moduleNode = new Node(NodeType::module, word->getPosition(), currentTable_);
	moduleNode = std::make_shared<Node>(NodeType::module, word->getPosition(), currentTable_);



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

//	currentTable_ = nullptr;
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
	std::string integer = std::string("CONSTANT");
	Symbol * symbol = currentTable_->getSymbol(&integer);
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
				auto temp = typeDef->getValue();
				Symbol* symbol = currentTable_->getSymbol(&temp);
				if (symbol == nullptr) {
					failUndeclaredSymbol(typeDef);
				}
				switch (symbol->getSymbolType()) {
				case SymbolType::array:
					addArray(identifier, typeDef, true);
					break;
				case SymbolType::record: {
					std::string temp2 = typeDef->getValue();
					Symbol* typeDefSymbol = currentTable_->getSymbol(&temp2);
					//std::shared_ptr<SymbolTable> typeDefTable = recordsSymbolTables_[typeDefSymbol];
					std::shared_ptr<SymbolTable> typeDefTable = currentTable_->getChild(*typeDefSymbol->getName());
					std::shared_ptr<SymbolTable> alias = typeDefTable->deepCopy(identifier->getValue());
					symbolTables_.push_back(alias);

					Symbol newSymbol = Symbol(identifier->getValue(), *typeDefSymbol->getTypes(), SymbolType::record, true);
					currentTable_->insert(newSymbol);
					currentTable_->addChild(alias);
					auto temp3 = identifier->getValue();
					Symbol* newSymbolsNewLocation = currentTable_->getSymbol(&temp3);
					recordsSymbolTables_.insert(std::unordered_map<Symbol*, std::shared_ptr<SymbolTable>>::value_type(newSymbolsNewLocation, alias));
				
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
    const Node* node = new Node(NodeType::number, word->getPosition(), std::to_string(number->getValue()), currentTable_);

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
				auto temp4 = identifier->getValue();
				Symbol* symbol = currentTable_->getSymbol(&temp4);
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

	// Creating symbolTable for the new lexicalScope in the new procedure.
	newSymbolTable(procIdent->getValue());
	std::vector<Symbol*> procTypes;


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
					procTypes.push_back(addType(identifier, typeDef, true));
					
				}
			}
					break;
			case (NodeType::array_type): {
				for (const Node* identifier : varIdentifiers->getChildren()) {
					procTypes.push_back(addArray(identifier, typeDef, true));
				}
			}
				break;
			case (NodeType::record_type): {
				for (const Node* identifier : varIdentifiers->getChildren()) {
					procTypes.push_back(addRecord(node, identifier, typeDef, true));
				}
			}
				break;
			}
		}
	}

	// Add symbol for procedure.
	Symbol procSymbol = Symbol(procIdent->getValue(), procTypes, SymbolType::procedure, true);
	if (currentTable_->getParent()->insert(procSymbol)) {
		failSymbolExists(&procSymbol);
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
	bool nextIsVar = scanner_->peekToken()->getType() == TokenType::kw_var;
	Node* node = nextIsVar ? new Node(NodeType::fp_section, word->getPosition(), "true", currentTable_)
		                   : new Node(NodeType::fp_section, word->getPosition(), "false", currentTable_);

	if (nextIsVar) {
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
{
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
			auto temp5 = identifier->getValue();
			Symbol* symbol = currentTable_->getSymbol(&temp5);
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
		if (procedure->getNodeType() == NodeType::expression) {
			failProcCallReferencedAsArray(identifier);
		}
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

std::shared_ptr<std::vector<const Node*>> Parser::selector(const Node * preceedingIdentifier) {
	auto selectors = std::make_shared<std::vector<const Node*>>();
	auto preceedingIdentifiers = std::vector<const Node*>();
	preceedingIdentifiers.push_back(preceedingIdentifier);
	auto temp6 = std::vector<TokenType>();
	std::vector<TokenType>* tokenTypes = &temp6;
	
	const Node* select = preceedingIdentifier;
	TokenType peekTokenType = scanner_->peekToken()->getType();
	while (peekTokenType == TokenType::period
		|| peekTokenType == TokenType::lbrack) {
		// Get select and add it to return values
		select = B();
		selectors->push_back(select);
		tokenTypes->push_back(peekTokenType);

		peekTokenType = scanner_->peekToken()->getType();

		if (peekTokenType == TokenType::period
			|| peekTokenType == TokenType::lbrack) {
			preceedingIdentifier = select->getChildren().at(0);
			preceedingIdentifiers.push_back(preceedingIdentifier);
		}
	}

	std::shared_ptr<SymbolTable> table = currentTable_;
	for (int i = 0; i < (int)(selectors->size()) - 1; i++) {
		TokenType tokenType = (*tokenTypes)[i+1];
		const Node* select2 = (*selectors)[i];
		std::vector<const Node*> selectChildren = select2->getChildren();
		
		const Node* preceedingCandidate = selectChildren.at(0);

		//Symbol* preceedingSymbol = table->getSymbol(&preceedingIdentifiers[i]->getValue());
		//std::shared_ptr<SymbolTable> recordsTable = recordsSymbolTables_[preceedingSymbol];
		std::shared_ptr<SymbolTable>  recordsTable = table->getChild(preceedingIdentifiers[i]->getValue());
		auto temp7 = preceedingCandidate->getValue();
		Symbol* candidateSymbol = recordsTable->getSymbol(&temp7);

		failUndeclaredSymbol(candidateSymbol, preceedingCandidate);

		switch (tokenType) {
		case TokenType::period:
			// Next after candidate is period, so the candidate must be a record.
			failIfNotARecord(preceedingCandidate, recordsTable);
			table = table->getChild(preceedingIdentifiers[i]->getValue());

			//table = recordsSymbolTables_[candidateSymbol];
			break;
		case TokenType::lbrack:
			// Next after candidate is expression, so this must be an array
			failIfNotAArray(preceedingCandidate, recordsTable);
			break;
		}

	}

	return selectors;
}

const Node* Parser::B() {
	Node* node = new Node(NodeType::selector, word->getPosition(), currentTable_);

	if (scanner_->peekToken()->getType() == TokenType::period) {
		point_t();

		node->addChild(ident());
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
		return new Node(NodeType::eq, word->getPosition(), "=", currentTable_);
	}
	else if (type == TokenType::op_neq) {
		return new Node(NodeType::neq, word->getPosition(), "#", currentTable_);
	}
	else if (type == TokenType::op_lt) {
		return new Node(NodeType::lt, word->getPosition(), "<", currentTable_);
	}
	else if (type == TokenType::op_leq) {
		return new Node(NodeType::leq, word->getPosition(), "<=", currentTable_);
	}
	else if (type == TokenType::op_gt) {
		return new Node(NodeType::gt, word->getPosition(), ">", currentTable_);
	}
	else if (type == TokenType::op_geq) {
		return new Node(NodeType::geq, word->getPosition(), ">=", currentTable_);
	}
	else if (type == TokenType::op_plus) {
		return new Node(NodeType::plus, word->getPosition(), "+", currentTable_);
	}
	else if (type == TokenType::op_minus) {
		return new Node(NodeType::minus, word->getPosition(), "-", currentTable_);
	}
	else if (type == TokenType::op_or) {
		return new Node(NodeType::or, word->getPosition(), "OR", currentTable_);
	}
	else if (type == TokenType::op_times) {
		return new Node(NodeType::times, word->getPosition(), "*", currentTable_);
	}
	else if (type == TokenType::op_div) {
		return new Node(NodeType::div, word->getPosition(), "DIV", currentTable_);
	}
	else if (type == TokenType::op_mod) {
		return new Node(NodeType::mod, word->getPosition(), "MOD", currentTable_);
	}
	else if (type == TokenType::op_and) {
		return new Node(NodeType::and, word->getPosition(), "&", currentTable_);
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
	auto temp8 = identifier->getValue();
	Symbol* undeclaredSymbol = currentTable_->getSymbol(&temp8);
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
	auto temp9 = identifier->getValue();
	Symbol* possibleRecord = symbolTable->getSymbol(&temp9);
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
	auto temp10 = identifier->getValue();
	if (currentTable_->getSymbol(&temp10)->getSymbolType() == SymbolType::procedure) {
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
	auto temp11 = identifier->getValue();
	Symbol* symbol = currentTable_->getSymbol(&temp11);
	failIfNotAVariable(symbol);
}

void Parser::failTypeCheckBinary(Symbol * a, Symbol * b, const Node * op)
{
	std::stringstream ss;
	ss << "Types are not appropriate for " << *a->getName() << ", " << *b->getName() << ": " << op->getValue();
	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed!: " + ss.str());
}

void Parser::failConstType(const Node * identifier, const Node * expression)
{
	std::stringstream ss;
	ss << "Failed to assign constant on " << identifier->getValue() << ". ";
	ss << "Expression must evaluate to constant:" << std::endl;
	ss << *expression;

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failLeftHandNotVariable(const Node * identifier)
{
	std::stringstream ss;
	ss << identifier->getValue() << " is not a variable";

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failTypeCheckAssignment(const Node * var, const Node * expression)
{
	std::stringstream ss;
	ss << var->getValue() + " and " << std::endl << *expression << " are not of the same type";

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failWrongParamCount(const Node * calledFunction, size_t formalCount, size_t actualCount)
{
	std::stringstream ss;
	ss << calledFunction->getValue() << " takes " << formalCount << " Arguments but " << actualCount << " were given.";

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::wrongActualParams(const Node * calledFunction, Symbol * formalParam, Symbol * actualParam)
{
	std::stringstream ss;
	ss << "Call to: " << calledFunction->getValue() << ". Parameter incompatibility: Was: " << *actualParam << " but should have been " << *formalParam;

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failNotABoolean(const Node * expression)
{
	std::stringstream ss;
	ss << "Expression does not evaluate to a boolean:" << std::endl;
	ss << *expression;

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failWrongArrayDimensions(const Node * expression, int dimension)
{
	std::stringstream ss;
	ss << "Array dimensions cannot be smaller than 1, but was: " << dimension << std::endl;
	ss << *expression;

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failArrayDimensionIsNotAConstant(const Node * expression)
{
	std::stringstream ss;
	ss << "Array Dimension is not a constant:" << std::endl;
	ss << *expression;

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::failProcCallReferencedAsArray(const Node * identifier)
{
	std::stringstream ss;
	ss << "Procedure referenced as array: " << identifier->getValue();

	logger_->error(word->getPosition(), ss.str());
	throw std::invalid_argument("You failed! " + ss.str());
}

void Parser::newSymbolTable(std::string name)
{
	std::shared_ptr<SymbolTable> newTable = currentTable_->nestedTable(currentTable_, name);
	symbolTables_.push_back(newTable);
	currentTable_ = newTable;
}

Symbol* Parser::addType(const Node * identifier, const Node * typeDef, bool asVariable)
{		
	// New type is an "alias" of an existing type. Check if that existing type exists
	auto temp12 = typeDef->getValue();
	Symbol* type = currentTable_->getSymbol(&temp12);
	failUndeclaredSymbol(type, typeDef);
	// The type exists. Add the alias of that type to the symbol table.
	std::vector<Symbol*> types;
	types.push_back(type);
	Symbol newAlias = Symbol(identifier->getValue(), types, SymbolType::type, asVariable);
	if (currentTable_->insert(newAlias)) {
		failSymbolExists(&newAlias);
	}

	auto temp = identifier->getValue();
	return currentTable_->getSymbol(&temp);
}

Symbol* Parser::addArray(const Node * identifier, const Node * typeDef, bool asVariable)
{
	// New type is an array. Check if the specified array type exists.
	const Node* typeDef2 = typeDef->getChildren().at(1)->getChildren().at(0);
	auto temp13 = typeDef2->getValue();
	Symbol* type = currentTable_->getSymbol(&temp13);
	failUndeclaredSymbol(type, typeDef2);
	failIfNotAType(type);

	// The type exists. Add the array of that type to the symbol table.
	std::vector<Symbol*> types;
	types.push_back(type);
	Symbol newArray = Symbol(identifier->getValue(), types, SymbolType::array, asVariable);
	if (currentTable_->insert(newArray)) {
		failSymbolExists(&newArray);
	}

	auto temp = identifier->getValue();
	return currentTable_->getSymbol(&temp);
}

Symbol* Parser::addRecord(const Node* node, const Node * identifier, const Node * typeDef, bool asVariable)
{
	// New lexical scope
	newSymbolTable(identifier->getValue());

	// New Type is a record. Check if all specified types exist.
	std::vector<Symbol*> recordTypes;
	std::vector<const Node*> fieldLists = typeDef->getChildren();
	for (const Node* fieldList : fieldLists) {
		// Get the symbol of the type of this field list
		const Node* typeOfType = fieldList->getChildren().at(1)->getChildren().at(0);
		auto temp14 = typeOfType->getValue();
		Symbol* typeOfTypeSymbol = currentTable_->getSymbol(&temp14);

//		const Node* identifierList = fieldList->getChildren().at(0);
		const Node* typeNode2 = fieldList->getChildren().at(1);
		const Node* typeIdentifier = typeNode2->getChildren().at(0);
		auto temp15 = typeIdentifier->getValue();
		Symbol* type = currentTable_->getSymbol(&temp15);
		std::vector<Symbol*> types;
		types.push_back(type);
		failUndeclaredSymbol(type, typeIdentifier);
		failIfNotAType(type);

		// Type exists. Add the identifiers.
		// If typeDef (the type of the newly added identifiers) is a record add the already existing Symbols, if not add new symbols.
		std::vector<const Node*> identifiers = fieldList->getChildren().at(0)->getChildren();

		switch (typeOfTypeSymbol->getSymbolType()) {
		case SymbolType::record: {
			std::shared_ptr<SymbolTable> typeOfTypeSymbolTable = recordsSymbolTables_[typeOfTypeSymbol];

			for (const Node* ident : identifiers) {
				std::shared_ptr<SymbolTable> copy = typeOfTypeSymbolTable->deepCopy(ident->getValue());
				symbolTables_.push_back(copy);
				currentTable_->addChild(copy);

				auto temp16 = typeOfType->getValue();
				Symbol* identSymbol = currentTable_->getSymbol(&temp16);//typeOfTypeSymbolTable->getSymbol(&ident->getValue());
				std::vector<Symbol*> types2;
				types2.push_back(identSymbol);
				Symbol newIdent(ident->getValue(), types2, SymbolType::record, asVariable);
				if (currentTable_->insert(newIdent)) {
					failSymbolExists(identSymbol);
				}

				recordTypes.push_back(type);
			}
		}
			break;
		case SymbolType::array:
			for (const Node* ident : identifiers) {
				Symbol newArray = typeOfTypeSymbol->copy(ident->getValue());
				if (currentTable_->insert(newArray)) {
					failSymbolExists(&newArray);
				}
				recordTypes.push_back(type);
			}
			break;
		case SymbolType::type:
			for (const Node* ident : identifiers) {
				Symbol newIdent(ident->getValue(), types, SymbolType::type, true);
				if (currentTable_->insert(newIdent)) {
					failSymbolExists(&newIdent);
				}
				recordTypes.push_back(type);
			}
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

	auto temp = identifier->getValue();
	return currentTable_->getSymbol(&temp);
}

std::string Parser::postParserTypeCheck(const Node * module, std::string lastIdent)
{
	if (module->getNodeType() == NodeType::identifier) {
		lastIdent = module->getValue();
	}

	auto oldTable = currentTable_;
	if (module->getNodeType() == NodeType::procedure_declaration) {
		auto next = currentTable_->getChild(module->getChildren().at(0)->getChildren().at(0)->getValue());
		if (next != nullptr) {
			currentTable_ = next;
		}
	}

	for (auto child : module->getChildren()) {
		switch (child->getNodeType()) {
		case NodeType::const_declarations:
			checkConstDeclType(child);
			break;
		case NodeType::assignment:
			checkAssignmentType(child);
			break;
		case NodeType::selector:
			// If an array is selected check wether the expression evaluates to a non-negative integer that is in range.
			break;
		case NodeType::procedure_call:
			// Check wether the given parameters are of an appropriate type for the referenced procedure.
			checkProcedureCallTypes(child);
			break;
		case NodeType::if_statement:
			// Check wether the expression evaluates to a boolean
			checkIfStatementType(child);
			break;
		case NodeType::while_statement:
			// Check wether the expression evaluates to a boolean
			checkWhileStatementType(child);
			break;
		case NodeType::array_type:
			// Check wether expression evaluates to a non-negative constant.
			int arrayDimension = checkArrayType(child);
			Symbol* arraySymbol = currentTable_->getSymbol(&lastIdent);
			arraySymbol->setValue(arrayDimension);
			break;
		}
		lastIdent = postParserTypeCheck(child, lastIdent);
	}

	currentTable_ = oldTable;

	return lastIdent;
}

Symbol * Parser::typeOfExpression(const Node * expression)
{
	auto nodeTypesA = std::vector<NodeType>();
	nodeTypesA.push_back(NodeType::eq);
	nodeTypesA.push_back(NodeType::neq);
	nodeTypesA.push_back(NodeType::lt);
	nodeTypesA.push_back(NodeType::leq);
	nodeTypesA.push_back(NodeType::gt);
	nodeTypesA.push_back(NodeType::geq);

	auto nodeTypesB = std::vector<NodeType>();

	return binaryTypeChecker(expression, NodeType::simple_expression, nodeTypesA, nodeTypesB);
}

Symbol * Parser::typeOfSimpleExpression(const Node * simpleExpression)
{
	// TODO throw one operator away.
	auto nodeTypesA = std::vector<NodeType>();
	nodeTypesA.push_back(NodeType::plus);
	nodeTypesA.push_back(NodeType::minus);

	auto nodeTypesB = std::vector<NodeType>();
	nodeTypesB.push_back(NodeType::or);

	return binaryTypeChecker(simpleExpression, NodeType::term, nodeTypesA, nodeTypesB);
}

Symbol * Parser::typeOfTerm(const Node * term)
{
	auto nodeTypesA = std::vector<NodeType>();
	nodeTypesA.push_back(NodeType::times);
	nodeTypesA.push_back(NodeType::div);
	nodeTypesA.push_back(NodeType::mod);

	auto nodeTypesB = std::vector<NodeType>();
	nodeTypesB.push_back(NodeType::and);

	return binaryTypeChecker(term, NodeType::factor, nodeTypesA, nodeTypesB);
}



Symbol * Parser::binaryTypeChecker(const Node * expSexpFact, NodeType sub, std::vector<NodeType> nodeTypesA, std::vector<NodeType> nodeTypesB)
{
	std::vector<const Node*> children = expSexpFact->getChildren();
	std::list<const Node*> operators;
	std::list<Symbol* > typesOfSubs;

	for (const Node* child : children) {
		if (child->getNodeType() == sub) {
			switch (sub) {
			case NodeType::simple_expression:
				typesOfSubs.push_back(typeOfSimpleExpression(child));
				break;
			case NodeType::factor:
				typesOfSubs.push_back(typeOfFactor(child));
				break;
			case NodeType::term:
				typesOfSubs.push_back(typeOfTerm(child));
			}
			
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	if (operators.size() == typesOfSubs.size()) {
		operators.pop_front();
	}

	while (typesOfSubs.size() > 1) {
		Symbol* typeA = typesOfSubs.front();
		typesOfSubs.pop_front();
		Symbol* typeB = typesOfSubs.front();
		typesOfSubs.pop_front();
		const Node* op = operators.front();
		operators.pop_front();

		if (*typeA != *typeB) {
			if (*typeA->getName() == std::string("INTEGER")) {
				if (*typeB->getName() != std::string("CONSTANT")) {
					failTypeCheckBinary(typeA, typeB, op);
				}
			}
			else if (*typeB->getName() == std::string("CONSTANT")) {
				if (*typeB->getName() != std::string("INTEGER")) {
					failTypeCheckBinary(typeA, typeB, op);
				}
			}
		}

		if (std::find(nodeTypesA.begin(), nodeTypesA.end(), op->getNodeType()) != nodeTypesA.end()) {
			if (*typeA->getName() != "INTEGER" && *typeA->getName() != "CONSTANT") {
				failTypeCheckBinary(typeA, typeB, op);
			}
			else {
				std::string temp17;
				if (sub == NodeType::simple_expression) {
					temp17 = std::string("BOOLEAN");
				}
				else if (*typeA->getName() == "INTEGER" || *typeB->getName() == "INTEGER") {
					temp17 = std::string("INTEGER");
				}
				else {
					temp17 = std::string("CONSTANT");
				}
				typesOfSubs.push_front(symbolTables_.at(0)->getSymbol(&temp17));
			}
		}
		else {
			if (*typeA->getName() != "BOOLEAN") {
				failTypeCheckBinary(typeA, typeB, op);
			}
			else {
				auto temp18 = std::string("BOOLEAN");
				typesOfSubs.push_front(symbolTables_.at(0)->getSymbol(&temp18));
			}
		}
	}

	return typesOfSubs.front();
}

void Parser::checkConstDeclType(const Node * node)
{
	const Node* identifier = node->getChildren().at(0);
	const Node* expression = node->getChildren().at(1);
	Symbol* type = typeOfExpression(expression);

	// Check wether the expression evaluates to a constant.
	std::string* typeName = type->getName();
	if (*typeName != "CONSTANT") {
		failConstType(identifier, expression);
	}

	// Save the constants value for later use.
	int expVal = evaluateExpression(expression);
	auto temp = identifier->getValue();
	Symbol* nodeSymbol = identifier->getSymbolTable()->getSymbol(&temp);
	nodeSymbol->setValue(expVal);
}

void Parser::checkAssignmentType(const Node * node)
{
	std::vector<const Node*> children = node->getChildren();
	std::shared_ptr<SymbolTable> tableOfVar = currentTable_;
	const Node* var = lastSelectorVariable(&children, &tableOfVar);

	auto temp = var->getValue();
	Symbol* varSymb = tableOfVar->getSymbol(&temp);

	if (!varSymb->isVariable()) {
		failLeftHandNotVariable(var);
	}

	const Node* expression = children.back();
	std::vector<Symbol*>* varTypes = varSymb->getTypes();
	Symbol* varType = varTypes->at(0);
	Symbol* expressionType = typeOfExpression(expression);

	if (varTypes->size() > 1) {
		failTypeCheckAssignment(var, expression);
	}
	if (varType != expressionType) {
		if (*varType->getName() != "INTEGER" || *expressionType->getName() != "CONSTANT") {
			failTypeCheckAssignment(var, expression);
		}
	}
}

/*void Parser::checkSelectorType(const Node * node)
{
	std::vector<const Node*> children = node->getChildren();
	const Node* expression = children.at(0);

	if (expression->getNodeType() != NodeType::expression) {
		return;
	}

	if (typeOfExpression(expression)->getTypes()->at(0)->getName() != "CONSTANT") {
		failConstType
	}
}*/

void Parser::checkProcedureCallTypes(const Node * node)
{
	std::vector<const Node*> callChildren = node->getChildren();
	const Node* procedureIdentifier = callChildren.at(0);

	std::vector<const Node*> actParamExpressions;
	std::vector<Symbol*> actParamTypes;
	if (callChildren.size() > 1) {
		actParamExpressions = callChildren.at(1)->getChildren();
		for (size_t i = 0; i < actParamExpressions.size(); i++) {
			actParamTypes.push_back(typeOfExpression(actParamExpressions.at(i)));
		}
	}

	auto temp = procedureIdentifier->getValue();
	Symbol* procedureDecl = node->getSymbolTable()->getSymbol(&temp);
	std::vector<Symbol*> formParamTypes = *procedureDecl->getTypes();

	size_t formalCount = formParamTypes.size();
	size_t actualCount = actParamTypes.size();
	if (formalCount != actualCount) {
		failWrongParamCount(procedureIdentifier, formalCount, actualCount);
	}

	for (size_t i = 0; i < formalCount; i++) {
		if (*formParamTypes[i]->getTypes()->at(0) != *actParamTypes[i]) {
			//if (*formParamTypes[i]->getTypes()->at(0)->getName() != "INTEGER" && *actParamTypes[i]->getName() != "CONSTANT") {
				wrongActualParams(procedureIdentifier, formParamTypes[i]->getTypes()->at(0), actParamTypes[i]);
			//}
		}
	}
}

void Parser::checkIfStatementType(const Node * node)
{
	for (auto child : node->getChildren()) {
		if (child->getNodeType() == NodeType::expression) {
			Symbol* type = typeOfExpression(child);

			if (*type->getName() != "BOOLEAN") {
				failNotABoolean(child);
			}
		}
	}
}


void Parser::checkWhileStatementType(const Node * node)
{
	const Node* expression = node->getChildren().at(0);
	Symbol* type = typeOfExpression(expression);

	if (*type->getName() != "BOOLEAN") {
		failNotABoolean(expression);
	}
}

int Parser::checkArrayType(const Node * node)
{
	const Node* expression = node->getChildren().at(0);

	if (*typeOfExpression(expression)->getName() != "CONSTANT") {
		failArrayDimensionIsNotAConstant(expression);
	}
	
	int dimensions = evaluateExpression(expression);
	if (dimensions < 1) {
		failWrongArrayDimensions(expression, dimensions);
	}

	return dimensions;
}

int Parser::evaluateExpression(const Node * node)
{
	std::vector<const Node*> children = node->getChildren();
	std::list<const Node*> operators;
	std::list<const Node*> simpleExpressions;


	for (const Node* child : children) {
		if (child->getNodeType() == NodeType::simple_expression) {
			simpleExpressions.push_back(child);
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	int returnVal = evaluateSimpleExpression(simpleExpressions.front());
	simpleExpressions.pop_front();
	while (simpleExpressions.size() > 0) {
		const Node* op = operators.front();
		const Node* se = simpleExpressions.front();
		operators.pop_front();
		simpleExpressions.pop_front();
		int seValue = evaluateSimpleExpression(se);

		switch (op->getNodeType()) {
		case NodeType::eq:
			returnVal = returnVal == seValue;
		case NodeType::neq:
			returnVal = returnVal != seValue;
		case NodeType::lt:
			returnVal = returnVal < seValue;
		case NodeType::leq:
			returnVal = returnVal <= seValue;
		case NodeType::gt:
			returnVal = returnVal > seValue;
		case NodeType::geq:
			returnVal = returnVal >= seValue;
		}
	}

	return returnVal;
}

int Parser::evaluateSimpleExpression(const Node * node)
{
	// TODO Code duplication
	std::vector<const Node*> children = node->getChildren();
	std::list<const Node*> operators;
	std::list<const Node*> terms;

	for (const Node* child : children) {
		if (child->getNodeType() == NodeType::term) {
			terms.push_back(child);
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	int returnVal = evaluateTerm(terms.front());
	terms.pop_front();
	// Check if there were a leading sign.
	if (terms.size() + 1 == operators.size()) {
		if (operators.front()->getNodeType() == NodeType::minus) {
			returnVal *= -1;
		}
		operators.pop_front();
	}
	while (terms.size() > 0) {
		const Node* op = operators.front();
		const Node* se = terms.front();
		operators.pop_front();
		terms.pop_front();
		int seValue = evaluateTerm(se);

		switch (op->getNodeType()) {
		case NodeType::plus:
			returnVal += seValue;
		case NodeType::minus:
			returnVal -= seValue;
		case NodeType::or:
			returnVal |= seValue;
		}
	}

	return returnVal;
}

int Parser::evaluateTerm(const Node * node)
{
	// TODO Code duplication
	std::vector<const Node*> children = node->getChildren();
	std::list<const Node*> operators;
	std::list<const Node*> factors;

	for (const Node* child : children) {
		if (child->getNodeType() == NodeType::factor) {
			factors.push_back(child);
		}
		else if (child->isBinaryOp()) {
			operators.push_back(child);
		}
		else {
			throw std::invalid_argument("Something failed horrible");
		}
	}

	int returnVal = evaluateFactor(factors.front());
	factors.pop_front();
	while (factors.size() > 0) {
		const Node* op = operators.front();
		const Node* se = factors.front();
		operators.pop_front();
		factors.pop_front();
		int seValue = evaluateFactor(se);

		switch (op->getNodeType()) {
		case NodeType::times:
			returnVal *= seValue;
			break;
		case NodeType::div:
			returnVal /= seValue;
			break;
		case NodeType::mod:
			returnVal %= seValue;
			break;
		case NodeType::and:
			returnVal &= seValue;
			break;
		}
	}

	return returnVal;
}

int Parser::evaluateFactor(const Node * node)
{
	std::vector<const Node*> children = node->getChildren();

	const Node* child = children.at(0);
	switch (child->getNodeType()) {
	case NodeType::identifier: {
		auto table = currentTable_;
		auto ident = lastSelectorVariable(&children, &table);
		auto identValue = ident->getValue();
		Symbol* identSymbol = table->getSymbol(&identValue);
		return identSymbol->getValue();
	}
		break;
	case NodeType::number: 
		return evaluateNumber(child);
		break;
	case NodeType::expression:
		return evaluateExpression(child);
		break;
	case NodeType::factor:
		return evaluateExpression(child);
		break;
	default:
		throw std::exception();
		break;
	}
}

int Parser::evaluateSelector(const Node * node)
{
	throw std::invalid_argument("Selectors may not be evaluated at compile time " + node->getValue());
}

int Parser::evaluateIdentifier(const Node * node)
{
	auto symbolTable = node->getSymbolTable();
	auto temp = node->getValue();
	Symbol* symbol = symbolTable->getSymbol(&temp);
	return symbol->getValue();
}

int Parser::evaluateNumber(const Node * node)
{
	return atoi(node->getValue().c_str());
}

const Node * Parser::lastSelectorVariable(std::vector<const Node*>* children, std::shared_ptr<SymbolTable>* table)
{
	const Node* lastSelector = children->at(0);
	std::string lastSelName = lastSelector->getValue();
	auto possibleChildSymbolTable = (*table)->getChild(lastSelName);
	if (possibleChildSymbolTable != nullptr) {
		*table = possibleChildSymbolTable;
	}

	for (auto possibleSelector : *children) {
		if (possibleSelector->getNodeType() == NodeType::selector
			&& possibleSelector->getChildren().at(0)->getNodeType() == NodeType::identifier) {
			lastSelector = possibleSelector->getChildren().at(0);

			lastSelName = lastSelector->getValue();
			possibleChildSymbolTable = (*table)->getChild(lastSelName);
			if (possibleChildSymbolTable != nullptr) {
				*table = possibleChildSymbolTable;
			}
		}
	}

	return lastSelector;
}

Symbol * Parser::typeOfFactor(const Node * factor)
{
	std::vector<const Node*> children = factor->getChildren();

	const Node* child = children.at(0);
	switch (child->getNodeType()) {
	case NodeType::identifier: {
		auto table = currentTable_;
		const Node* lastSelector = lastSelectorVariable(&children, &table);

		auto oldTable = currentTable_;
		currentTable_ = table;
		Symbol* returnVal = typeOfSelector(lastSelector);
		currentTable_ = oldTable;
		return returnVal;
	}
		break;
	case NodeType::number: {
		auto temp19 = std::string("CONSTANT");
		return symbolTables_.front()->getSymbol(&temp19);
	}
		break;
	case NodeType::expression:
		return typeOfExpression(child);
		break;
	case NodeType::factor:
		return typeOfFactor(child);
		break;
	default:
		return nullptr;
		break;
	}
}

Symbol * Parser::typeOfSelector(const Node * selector)
{
	std::vector<const Node*> children = selector->getChildren();
	if (children.size() == 0) {
		if (selector->getNodeType() == NodeType::identifier) {
			return typeOfIdentifier(selector);
		}
		else {
			return nullptr;
		}
	}

	const Node* child = children.at(0);
	switch (child->getNodeType()) {
	case NodeType::identifier:
		return typeOfIdentifier(child);
		break;
	case NodeType::expression:
		return typeOfExpression(child);
		break;
	default:
		return nullptr;
		break;
	}
}

Symbol * Parser::typeOfIdentifier(const Node * identifier)
{
	//std::shared_ptr<SymbolTable> st = identifier->getSymbolTable();
	auto temp20 = identifier->getValue();
	Symbol* symbol = currentTable_->getSymbol(&temp20);
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
