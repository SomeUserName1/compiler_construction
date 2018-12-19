#include "SymbolTable.h"

// Creates a SymbolTable for the outermost lexical scope
SymbolTable::SymbolTable() {
	level_ = 0;
	parent_ = nullptr;

	this->insert(Symbol("INTEGER", std::vector<Symbol *>(), SymbolType::variable));
	this->insert(Symbol("BOOLEAN", std::vector<Symbol *>(), SymbolType::variable));
	//this->insert(Symbol("ARRAY", std::vector<Symbol *>(), SymbolType::variable));
	this->insert(Symbol("CONSTANT", std::vector<Symbol *>(), SymbolType::variable)); // Kann vielleicht auch eher raus
}

// Creates a SymbolTable for a child scope of another lexical scope.
SymbolTable::SymbolTable(SymbolTable * parent) {
	level_ = parent->getLevel() + 1;
	parent_ = parent;
}

// Creates a new SymbolTable as a child of the current one.
SymbolTable SymbolTable::nestedTable() {
	return SymbolTable(this);
}

size_t SymbolTable::getLevel() {
	return level_;
}

Symbol * SymbolTable::getSymbol(std::string * ident) {
	auto elem = symbolTable_.find(*ident);

	if (elem == symbolTable_.end()) {
		// Symbol not in this table. Check parents table if it exists.
		if (parent_ == nullptr) {
			// This is the root (level 0), return nullptr to indicate failure.
			return nullptr;
		}
		else {
			// Recursively ask parent for the symbol
			return parent_->getSymbol(ident);
		}
	}
	else {
		// Symbol is in this table. Return it.
		return &elem->second;
	}
}

int SymbolTable::insert(Symbol symbol) {
	auto name = symbol.getName();
	auto elem = symbolTable_.find(*name);

	if (elem == symbolTable_.end()) {
		// Symbol does already exist.
		return 1;
	}

	symbolTable_[*name] = symbol;
	return 0;
}

