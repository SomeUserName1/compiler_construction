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

	std::vector<std::shared_ptr<SymbolTable>> symbolTables_;
	std::shared_ptr<SymbolTable> currentTable_;
	std::unordered_map<Symbol*, std::shared_ptr<SymbolTable>> recordsSymbolTables_;
	std::shared_ptr<Node> moduleNode;



	// None-Terminals
    const std::shared_ptr<Node> module();
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
	std::shared_ptr<std::vector<const Node*>> selector(const Node * preceedingIdentifier);
	const Node* ident();
	const Node* number();

	// Added non-terminals
	const Node* binary_op();
	const Node* A();
	const Node* B();

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
	void failUndeclaredSymbol(Symbol * undeclaredSymbol, const Node *identifier);
	void failUndeclaredSymbol(const Node *identifier);
	void failIfNotAType(Symbol *identifier);
	void failSymbolExists(Symbol * symbol);
	void failIfNotASomething(const Node * identifier, SymbolType symbolType);
	void failIfNotASomething(const Node * identifier, SymbolType symbolType, std::shared_ptr<SymbolTable> symbolTable);
	void failIfNotARecord(const Node * identifier);
	void failIfNotARecord(const Node * identifier, std::shared_ptr<SymbolTable> symbolTable);
	void failIfNotAArray(const Node * identifier);
	void failIfNotAArray(const Node * identifier, std::shared_ptr<SymbolTable> symbolTable);
	void failNetiherRecordNorArray(const Node * identifier);
	void failIfNotProcedure(const Node * identifier);
	void failIfNotAVariable(Symbol * variable);
	void failIfNotAVariable(const Node* identifier);
	void failTypeCheckBinary(Symbol* a, Symbol* b, const Node* op);
	void failConstType(const Node* identifier, const Node* expression);
	void failLeftHandNotVariable(const Node* identifier);
	void failTypeCheckAssignment(const Node* var, const Node* expression);
	void failWrongParamCount(const Node* calledFunction, size_t formalCount, size_t actualCount);
	void wrongActualParams(const Node* calledFunction, Symbol* formalParam, Symbol* actualParam);
	void failNotABoolean(const Node* expression);
	void failWrongArrayDimensions(const Node* expression, int dimension);

	// Helper methods for building the SymbolTables
	void newSymbolTable(std::string name);
	Symbol* addType(const Node* identifier, const Node* typeDef, bool asVariable);
	Symbol* addArray(const Node* identifier, const Node* typeDef, bool asVariable);
	Symbol* addRecord(const Node* node, const Node* identifier, const Node* typeDef, bool asVariable);

	Symbol* typeOfExpression(const Node* expression);
	Symbol* typeOfSimpleExpression(const Node* simpleExpression);
	Symbol* typeOfTerm(const Node* term);
	Symbol* typeOfFactor(const Node* factor);
	Symbol* typeOfSelector(const Node* selector);
	Symbol* typeOfIdentifier(const Node* identifier);
	Symbol* binaryTypeChecker(const Node* expSexpFact, NodeType sub, std::vector<NodeType> nodeTypesA, std::vector<NodeType> nodeTypesB);

	void postParserTypeCheck(const Node* module);
	void checkConstDeclType(const Node* node);
	void checkAssignmentType(const Node* node);
	//void checkSelectorType(const Node* node);
	void checkProcedureCallTypes(const Node* node);
	void checkIfStatementType(const Node* node);
	void checkWhileStatementType(const Node* node);
	void checkArrayType(const Node* node);

	int evaluateExpression(const Node* node);
	int evaluateSimpleExpression(const Node* node);
	int evaluateTerm(const Node* node);
	int evaluateFactor(const Node* node);
	int evaluateSelector(const Node* node);
	int evaluateIdentifier(const Node* node);
	int evaluateNumber(const Node* node);
	// TODO generic evaluator with function pointers

	const Node* lastSelectorVariable(std::vector<const Node*>* children, std::shared_ptr<SymbolTable>* table);

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<Node> parse();
};


#endif //OBERON0C_PARSER_H
