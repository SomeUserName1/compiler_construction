
#include <map>
#include "Symbol.h"



class SymbolTable {
private:
	std::map<std::string, Symbol> symbolTable_;

public:
	explicit SymbolTable();
};