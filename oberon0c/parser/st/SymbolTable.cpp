#include "SymbolTable.h"

SymbolTable::SymbolTable()
{
}

Symbol * SymbolTable::getSymbol(std::string * ident) {
	auto elem = symbolTable_.find(ident);

	if (elem == symbolTable_.end()) {
		return nullptr;
	}
	else {
		return &elem->second;
	}
}

int SymbolTable::insert(Symbol symbol)
{
	auto name = symbol.getName();
	if (getSymbol(name) != nullptr) {
		return 1;
	}

	symbolTable_[*name] = symbol;
	return 0;
}

