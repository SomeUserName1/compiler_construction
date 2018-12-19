
#include <unordered_map>
#include "Symbol.h"


class SymbolTable {
private:
	std::unordered_map<std::string, Symbol> symbolTable_;
	size_t level_;
	SymbolTable * parent_;

public:
	explicit SymbolTable();
	explicit SymbolTable(SymbolTable * parent);

	SymbolTable nestedTable();

	size_t getLevel();

	Symbol * getSymbol(std::string * ident);
	int insert(Symbol symbol);
};