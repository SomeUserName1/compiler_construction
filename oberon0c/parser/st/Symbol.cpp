#include <string>
#include "Symbol.h"

Symbol::Symbol()
{
	name_ = "default";
}

Symbol::Symbol(std::string name) {
	name_ = name;
}

std::string * Symbol::getName() {
	return &name_;
}
