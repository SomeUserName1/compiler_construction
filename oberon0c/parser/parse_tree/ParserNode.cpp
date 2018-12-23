#include "ParserNode.h"

ParserNode::ParserNode(ParserNodeType nodeType, FilePos pos) : nodeType_(nodeType), pos_(std::move(pos)) {}

ParserNode::ParserNode(ParserNodeType nodeType, FilePos pos, std::string value)
{
	nodeType_ = nodeType;
	pos_ = std::move(pos);
	value_ = std::move(value);
}

const ParserNodeType ParserNode::getParserNodeType() const
{
	return nodeType_;
}

std::ostream& operator<<(std::ostream &stream, const ParserNodeType &type) {
	std::string result;
	switch(type) {
		case ParserNodeType::module: result = "MODULE"; break;
		case ParserNodeType::declarations: result = "Declarations"; break;
		case ParserNodeType::const_declarations: result = "CONST declarations"; break;
		case ParserNodeType::type_declarations: result = "TYPE declarations"; break;
		case ParserNodeType::var_declarations: result = "var_declarations"; break;
		case ParserNodeType::procedure_declaration: result = "procedure_declaration"; break;
		case ParserNodeType::identifier: result = "identifier"; break;
		case ParserNodeType::number: result = "number"; break;
		case ParserNodeType::binary_op: result = "binary_op"; break;
		case ParserNodeType::expression: result = "expression"; break;
		case ParserNodeType::simple_expression: result = "simple_expression"; break;
		case ParserNodeType::term: result = "term"; break;
		case ParserNodeType::factor: result = "factor"; break;
		case ParserNodeType::record_type: result = "record_type"; break;
		case ParserNodeType::array_type: result = "array_type"; break;
		case ParserNodeType::field_list: result = "field_list"; break;
		case ParserNodeType::ident_list: result = "ident_list"; break;
		case ParserNodeType::procedure_heading: result = "procedure_heading"; break;
		case ParserNodeType::procedure_body: result = "procedure_body"; break;
		case ParserNodeType::formal_parameters: result = "formal_parameters"; break;
		case ParserNodeType::fp_section: result = "fp_section"; break;
		case ParserNodeType::statement_sequence: result = "statement_sequence"; break;
		case ParserNodeType::statement: result = "statement"; break;
		case ParserNodeType::if_statement: result = "if_statement"; break;
		case ParserNodeType::while_statement: result = "while_statement"; break;
		case ParserNodeType::acutal_parameters: result = "acutal_parameters"; break;
		case ParserNodeType::selector: result = "selector"; break;
		case ParserNodeType::assignment: result = "assignment"; break;
		case ParserNodeType::procedure_call: result = "procedure_call"; break;
	}
	stream << result;
	return stream;
}

void ParserNode::print(std::ostream & stream) const {
	if (!this->value_.empty()) {
		stream << this->getParserNodeType() << ": " << this->getValue();
	} else {
		stream << this->getParserNodeType();
	}
}

void ParserNode::printTree(std::ostream & stream) const {
	printTreeRec(stream, 0);
}

void ParserNode::printTreeRec(std::ostream & stream, int depth) const
{
	for (int i = 0; i < depth; i++) {
		stream << "|-";
	}

	this->print(stream);
	stream << std::endl;

	for (const auto &child : this->getChildren()) {
		dynamic_cast<ParserNode &>(*child).printTreeRec(stream, depth + 1);
	}
}

std::string ParserNode::getValue() const {
	return value_;
}

std::ostream & operator<<(std::ostream & stream, const ParserNode & node) {
	node.printTree(stream);

	return stream;
}
ParserNode::~ParserNode() = default;
