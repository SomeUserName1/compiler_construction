#ifndef OBERON0C_ST_H
#define OBERON0C_ST_H

#include <unordered_map>
#include <memory>
#include "Symbol.h"


class SymbolTable {
private:
	std::unordered_map<std::string, Symbol> symbolTable_;
	size_t level_;
	std::shared_ptr<SymbolTable> parent_;

public:
	explicit SymbolTable();
	explicit SymbolTable(std::shared_ptr<SymbolTable> parent);

	std::shared_ptr<SymbolTable> nestedTable(std::shared_ptr<SymbolTable> parent);

	size_t getLevel();

	Symbol * getSymbol(std::string * ident);
	int insert(Symbol symbol);
};
#endif