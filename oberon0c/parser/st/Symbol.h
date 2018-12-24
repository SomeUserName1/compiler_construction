#include <vector>
#include <ostream>
#include <string>

//enum class Type : char {
//	integer, boolean, array, constant
//;

enum class SymbolType : char {
	module, procedure, constant, record, array, type
};

class Symbol {

private:
	std::string name_;
	std::vector<Symbol *> types_;
	SymbolType symbolType_;
	bool isVariable_;

public:
	explicit Symbol();
	explicit Symbol(std::string name, std::vector<Symbol *> types, SymbolType symbolType, bool isVariable);

	std::string * getName();
	SymbolType getSymbolType() const;
	std::vector<Symbol *>* getTypes();

	bool isType() const;
	bool isVariable() const;

	Symbol copy(std::string name);

	void print(std::ostream & stream) const;
	friend std::ostream& operator<<(std::ostream &stream, const Symbol &node);

	bool operator==(Symbol other);
	bool operator!=(Symbol other);
};