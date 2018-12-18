

class Symbol {

private:
	std::string *name_;

public:
	explicit Symbol(std::string *name);
	virtual ~Symbol();
};