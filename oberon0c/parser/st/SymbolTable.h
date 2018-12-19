
#include <unordered_map>
#include "Symbol.h"


class SymbolTable {
private:
	std::unordered_map<std::string, Symbol> symbolTable_;

public:
	explicit SymbolTable();

	Symbol * getSymbol(std::string ident);
};