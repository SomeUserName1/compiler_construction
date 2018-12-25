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
    std::shared_ptr<Node> module();
    Node* declarations();
    Node* const_declarations();
    Node* type_declarations();
    Node* var_declarations();
    Node* procedure_declaration();
    Node* expression();
    Node* simple_expression();
    Node* term();
    Node* factor();
    Node* type();
    Node* array_type();
    Node* record_type();
    Node* field_list();
    Node* ident_list();
    Node* procedure_heading();
    Node* procedure_body();
    Node* formal_parameters();
    Node* fp_section();
    Node* statement_sequence();
    Node* statement();
    Node* if_statement();
    Node* while_statement();
    Node* actual_parameters();
    std::shared_ptr<std::vector<Node*>> selector(Node * preceedingIdentifier);
    Node* ident();
	Node* number();

	// Added non-terminals
	Node* binary_op();
	Node* A();
	Node* B();

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
	void failUndeclaredSymbol(Node *identifier);
	void failIfNotAType(Symbol *identifier);
	void failSymbolExists(Symbol * symbol);
	void failIfNotASomething(Node * identifier, SymbolType symbolType);
	void Parser::failIfNotASomething(Node * identifier, SymbolType symbolType, std::shared_ptr<SymbolTable> symbolTable);
	void failIfNotARecord(Node * identifier);
	void failIfNotARecord(Node * identifier, std::shared_ptr<SymbolTable> symbolTable);
	void failIfNotAArray(Node * identifier);
	void failIfNotAArray(Node * identifier, std::shared_ptr<SymbolTable> symbolTable);
	void failNetiherRecordNorArray(Node * identifier);
	void failIfNotProcedure(Node * identifier);
	void failIfNotAVariable(Symbol * variable);
	void failIfNotAVariable(Node* identifier);
	void failTypeCheckBinary(Symbol* a, Symbol* b, Node* op);
	void failConstType(Node* identifier, Node* expression);
	void failLeftHandNotVariable(Node* identifier);
	void failTypeCheckAssignment(Node* var, Node* expression);
	void failWrongParamCount(Node* calledFunction, size_t formalCount, size_t actualCount);
	void wrongActualParams(Node* calledFunction, Symbol* formalParam, Symbol* actualParam);

	// Helper methods for building the SymbolTables
	void newSymbolTable(std::string name);
	void addType(Node* identifier, Node* typeDef, bool asVariable);
	void addArray(Node* identifier, Node* typeDef, bool asVariable);
	void addRecord(Node* node, Node* identifier, Node* typeDef, bool asVariable);

	Symbol* typeOfExpression(Node* expression);
	Symbol* typeOfSimpleExpression(Node* simpleExpression);
	Symbol* typeOfTerm(Node* term);
	Symbol* typeOfFactor(Node* factor);
	Symbol* typeOfSelector(Node* selector);
	Symbol* typeOfIdentifier(Node* identifier);
	Symbol* binaryTypeChecker(Node* expSexpFact, NodeType sub, std::vector<NodeType> nodeTypesA, std::vector<NodeType> nodeTypesB);

	void postParserTypeCheck(Node* module);
	void checkConstDeclType(Node* node);
	void checkAssignmentType(Node* node);
	//void checkSelectorType(Node* node);
	void checkProcedureCallTypes(Node* node);
	void checkIfStatementType(Node* node);
	void checkElseIfStatementType(Node* node);
	void checkWhileStatementType(Node* node);
	void checkArrayType(Node* node);

	int evaluateExpression(Node* node);
	int evaluateSimpleExpression(Node* node);
	int evaluateTerm(Node* node);
	int evaluateFactor(Node* node);
	int evaluateSelector(Node* node);
	int evaluateIdentifier(Node* node);
	int evaluateNumber(Node* node);
	// TODO generic evaluator with function pointers

	Node* lastSelectorVariable(std::vector<Node*>* children, std::shared_ptr<SymbolTable>* table);

public:
    explicit Parser(Scanner *scanner, Logger *logger);
    ~Parser();
    const std::unique_ptr<Node> parse();
};


#endif //OBERON0C_PARSER_H
