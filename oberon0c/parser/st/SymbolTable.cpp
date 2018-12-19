#include "SymbolTable.h"

SymbolTable::SymbolTable()
{
	Symbol s = Symbol("lala");
	symbolTable_["lala"] = s;
}

Symbol * SymbolTable::getSymbol(std::string ident) {
	auto elem = symbolTable_.find(ident);

	if (elem == symbolTable_.end()) {
		return nullptr;
	}
	else {
		return &elem->second;
	}
}

