#ifndef OBERON0C_ST_H
#define OBERON0C_ST_H

#include <unordered_map>
#include <memory>
#include <vector>
#include "Symbol.h"


class SymbolTable {
private:
	std::unordered_map<std::string, Symbol> symbolTable_;
	size_t level_;
	std::shared_ptr<SymbolTable> parent_;
	std::vector<std::shared_ptr<SymbolTable>> children_;
	std::string name_;

	void printTreeRec(std::ostream &stream, int depth) const;

public:
	   
	explicit SymbolTable(std::string name);
	explicit SymbolTable(std::shared_ptr<SymbolTable> parent, std::string name);
	explicit SymbolTable(std::shared_ptr<SymbolTable> parent, std::string name, std::unordered_map<std::string, Symbol> symbolTable, std::vector<std::shared_ptr<SymbolTable>> children);
	~SymbolTable() = default;

	std::shared_ptr<SymbolTable> nestedTable(std::shared_ptr<SymbolTable> parent, std::string name);

	size_t getLevel();

	Symbol * getSymbol(std::string * ident);
	int insert(Symbol symbol);

	std::string getName();
	void addChild(std::shared_ptr<SymbolTable> child);
	std::shared_ptr<SymbolTable> getChild(std::string childsName);

	std::shared_ptr<SymbolTable> deepCopy(std::string name);

	void printTree(std::ostream &stream) const;
	friend std::ostream& operator<<(std::ostream &stream, SymbolTable &node);

	std::shared_ptr<SymbolTable> getParent();

	std::unordered_map<std::string, Symbol> getMap();
};
#endif