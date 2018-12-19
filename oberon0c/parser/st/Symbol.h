

class Symbol {

private:
	std::string name_;

public:
	explicit Symbol();
	explicit Symbol(std::string name);

	std::string * getName();
};