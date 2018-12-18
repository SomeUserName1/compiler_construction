#include "SymbolTable.h"

SymbolTable::SymbolTable()
{
	symbolTable_["lala"] = Symbol(&std::string("lala"));
}

