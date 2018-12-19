#include "SymbolTable.h"

SymbolTable::SymbolTable() {
	level_ = 0;
	parent_ = nullptr;
}

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
	if (getSymbol(name) != nullptr) {
		// Symbol does already exist.
		return 1;
	}

	symbolTable_[*name] = symbol;
	return 0;
}

