#include <vector>

//enum class Type : char {
//	integer, boolean, array, constant
//;

enum class SymbolType : char {
	module, procedure, constant, variable, record, array
};

class Symbol {

private:
	std::string name_;
	std::vector<Symbol *> types_;
	SymbolType symbolType_;

public:
	explicit Symbol();
	explicit Symbol(std::string name, std::vector<Symbol *> types, SymbolType symbolType);

	std::string * getName();
};