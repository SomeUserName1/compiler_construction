#include <string>
#include "Symbol.h"

Symbol::Symbol()
{
	throw  std::runtime_error("This constructor is only for the compiler not to whine.");
}

Symbol::Symbol(std::string name, std::vector<Type> types) {
	name_ = name;
	types_ = types;
}

std::string * Symbol::getName() {
	return &name_;
}
