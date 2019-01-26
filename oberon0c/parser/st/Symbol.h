#ifndef OBERON0C_SYMBOLOL_H
#define OBERON0C_SYMBOLOL_H

#include <vector>
#include <ostream>
#include <string>
#include <memory>

//enum class Type : char {
//	integer, boolean, array, constant
//;

enum class SymbolType : char {
	module, procedure, constant, record, array, type
};

class ASTNode;
class SymbolTable;

class Symbol {

private:
	std::string name_;
	std::vector<Symbol *> types_;
	SymbolType symbolType_;
	bool isVariable_;
	int value_;
	std::shared_ptr<ASTNode> ast_;
	bool isVarParam_;
	std::shared_ptr<SymbolTable> symbolTable_; //The table this symbol is standing in.

public:
	explicit Symbol();
	explicit Symbol(std::string name, std::vector<Symbol *> types, SymbolType symbolType, bool isVariable);
	explicit Symbol(std::string name, std::vector<Symbol *> types, SymbolType symbolType, bool isVariable, std::shared_ptr<SymbolTable> symbolTable);

	std::string * getName();
	SymbolType getSymbolType() const;
	std::vector<Symbol *>* getTypes();

	bool isType() const;
	bool isVariable() const;

	Symbol copy(std::string name);

	void print(std::ostream & stream) const;
	friend std::ostream& operator<<(std::ostream &stream, const Symbol &node);

	bool operator==(Symbol other);
	bool operator!=(Symbol other);

	void setValue(int value);
	int getValue();

	void setAst(const std::shared_ptr<ASTNode> node);
	const std::shared_ptr<ASTNode> getAst();

	void setIsVarParam(bool val);
	bool getIsVarParam();

	std::shared_ptr<SymbolTable> getSymbolTable();

	size_t size();
};
#endif