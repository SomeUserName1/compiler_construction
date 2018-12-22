#include <string>
#include "Symbol.h"

Symbol::Symbol()
{
	throw  std::runtime_error("This constructor is only for the compiler not to whine.");
}

Symbol::Symbol(std::string name, std::vector<Symbol *> types, SymbolType symbolType, bool isVariable) {
	name_ = name;
	types_ = types;
	symbolType_ = symbolType;
	isVariable_ = isVariable;
}

std::string * Symbol::getName() {
	return &name_;
}

SymbolType Symbol::getSymbolType() const
{
	return symbolType_;
}

std::vector<Symbol*>* Symbol::getTypes()
{
	return &types_;
}

bool Symbol::isType() const
{
	return !isVariable_;
}

bool Symbol::isVariable() const
{
	return isVariable_;
}
