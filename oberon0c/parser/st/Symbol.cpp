#include <string>
#include "Symbol.h"

Symbol::Symbol()
{
	throw  std::runtime_error("This constructor is only for the compiler not to whine.");
}

Symbol::Symbol(std::string name, std::vector<Symbol*> types, SymbolType symbolType, bool isVariable)
{
	name_ = name;
	types_ = types;
	symbolType_ = symbolType;
	isVariable_ = isVariable;
	value_ = 0;
}

Symbol::Symbol(std::string name, std::vector<Symbol *> types, SymbolType symbolType, bool isVariable, std::shared_ptr<SymbolTable> symbolTable) {
	name_ = name;
	types_ = types;
	symbolType_ = symbolType;
	isVariable_ = isVariable;
	value_ = 0;
	symbolTable_ = symbolTable;
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

Symbol Symbol::copy(std::string name)
{
	return Symbol(name, types_, symbolType_, isVariable_, symbolTable_);
}

void Symbol::print(std::ostream & stream) const
{
	stream << name_ << ": ";
	switch (symbolType_) {
	case SymbolType::array:
		stream << "Array";
		break;
	case SymbolType::constant:
		stream << "Constant";
		break;
	case SymbolType::module:
		stream << "Module";
		break;
	case SymbolType::procedure:
		stream << "Procedure";
		break;
	case SymbolType::record:
		stream << "Record";
		break;
	case SymbolType::type:
		stream << "Type";
		break;
	}

	if (isVariable_) {
		stream << " as Variable";
	}
	else {
		stream << " as TypeDef";
	}

	stream << " Value: " << value_;
}

bool Symbol::operator==(Symbol other)
{
	auto otherTypes = other.getTypes();
	if (types_.size() != otherTypes->size()) {
		return false;
	}

	if (types_.size() == 0) {
		return name_ == *other.getName();
	}

	for (size_t i = 0; i < types_.size(); i++) {
		if (types_.at(i) != otherTypes->at(i)) {
			return false;
		}
	}

	return true;
}

bool Symbol::operator!=(Symbol other)
{
	return !((*this) == other);
}

int Symbol::getValue()
{
	return value_;
}

void Symbol::setAst(const ASTNode * node)
{
	ast_ = node;
}

const ASTNode * Symbol::getAst()
{
	return ast_;
}

void Symbol::setIsVarParam(bool val)
{
	isVarParam_ = val;
}

bool Symbol::getIsVarParam()
{
	return isVarParam_;
}

std::shared_ptr<SymbolTable> Symbol::getSymbolTable()
{
	return symbolTable_;
}

void Symbol::setValue(int value)
{
	value_ = value;
}

std::ostream & operator<<(std::ostream & stream, const Symbol & symbol)
{
	symbol.print(stream);

	return stream;
}
