#include <vector>

enum class Type : char {
	integer, boolean, array, constant
};

enum class ScopeType : char {
	module, record, procedure
};

class Symbol {

private:
	std::string name_;
	std::vector<Type> types_;

public:
	explicit Symbol();
	explicit Symbol(std::string name, std::vector<Type> types);

	std::string * getName();
};