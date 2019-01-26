#include "SymbolTable.h"

void SymbolTable::printTreeRec(std::ostream & stream, int depth) const
{
	for (int i = 0; i < depth; i++) {
		stream << "|-";
	}
	stream << name_ << ":" << std::endl;

	depth++;

	for (auto pair : symbolTable_) {
		Symbol symbol = pair.second;
		for (int i = 0; i < depth; i++) {
			stream << "|-";
		}
		stream << symbol << std::endl;
	}

	for (auto child : children_) {
		child->printTreeRec(stream, depth);
	}
}

// Creates a SymbolTable for the outermost lexical scope
SymbolTable::SymbolTable(std::string name) {
	level_ = 0;
	parent_ = nullptr;
	name_ = name;

	this->insert(Symbol("INTEGER", std::vector<Symbol *>(), SymbolType::type, false));
	this->insert(Symbol("BOOLEAN", std::vector<Symbol *>(), SymbolType::type, false));
	//this->insert(Symbol("ARRAY", std::vector<Symbol *>(), SymbolType::type, false));
	this->insert(Symbol("CONSTANT", std::vector<Symbol *>(), SymbolType::type, false)); // Kann vielleicht auch raus
}

// Creates a SymbolTable for a child scope of another lexical scope.
SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> parent, std::string name) {
	level_ = parent->getLevel() + 1;
	parent_ = parent;
	name_ = name;
}

SymbolTable::SymbolTable(std::shared_ptr<SymbolTable> parent, std::string name, std::unordered_map<std::string, Symbol> symbolTable, std::vector<std::shared_ptr<SymbolTable>> children)
{
	level_ = parent->getLevel() + 1;
	parent_ = parent;
	name_ = name;
	symbolTable_ = symbolTable;
	children_ = children;
}

// Creates a new SymbolTable as a child of the current one.
std::shared_ptr<SymbolTable> SymbolTable::nestedTable(std::shared_ptr<SymbolTable> parent, std::string name) {
	auto newTable = std::make_shared<SymbolTable>(parent, name);

	if (parent != nullptr) {
		parent->addChild(newTable);
	}

	return newTable;
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

	if (elem != symbolTable_.end()) {
		// Symbol does already exist.
		return 1;
	}

	//symbolTable_[*name] = symbol;
	symbolTable_.insert(std::unordered_map< std::string, Symbol>::value_type(*name, symbol));
	return 0;
}

std::string SymbolTable::getName()
{
	return name_;
}

void SymbolTable::addChild(std::shared_ptr<SymbolTable> child)
{
	children_.push_back(child);
}

std::shared_ptr<SymbolTable> SymbolTable::getChild(std::string childsName)
{
	for (auto child : children_) {
		if (child->getName() == childsName) {
			return child;
		}
	}

	return nullptr;
}

std::shared_ptr<SymbolTable> SymbolTable::deepCopy(std::string name)
{
	return std::make_shared<SymbolTable>(parent_, name, symbolTable_, children_);
}

void SymbolTable::printTree(std::ostream & stream) const
{
	printTreeRec(stream, 0);
}

std::shared_ptr<SymbolTable> SymbolTable::getParent()
{
	return parent_;
}

std::unordered_map<std::string, Symbol> SymbolTable::getMap()
{
	return symbolTable_;
}

std::ostream & operator<<(std::ostream & stream, SymbolTable & node)
{
	node.printTree(stream);

	return stream;
}
