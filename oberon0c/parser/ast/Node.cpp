#include <utility>


#include "Node.h"

Node::Node(NodeType nodeType, FilePos pos, SymbolTable * symbolTable) : nodeType_(nodeType), pos_(std::move(pos)), symbolTable_(symbolTable) {}

Node::Node(NodeType nodeType, FilePos pos, std::string value, SymbolTable * symbolTable)
{
	nodeType_ = nodeType;
	pos_ = std::move(pos);
	value_ = std::move(value);
	symbolTable_ = symbolTable;
}

const NodeType Node::getNodeType() const
{
	return nodeType_;
}

const FilePos Node::getFilePos() const
{
	return pos_;
}

const std::vector<Node> Node::getChildren() const {
	return children_;
}

std::ostream& operator<<(std::ostream &stream, const NodeType &type) {
	std::string result;
	switch(type) {
		case NodeType::module: result = "MODULE"; break;
		case NodeType::declarations: result = "Declarations"; break;
		case NodeType::const_declarations: result = "CONST declarations"; break;
		case NodeType::type_declarations: result = "TYPE declarations"; break;
		case NodeType::var_declarations: result = "var_declarations"; break;
		case NodeType::procedure_declaration: result = "procedure_declaration"; break;
		case NodeType::identifier: result = "identifier"; break;
		case NodeType::number: result = "number"; break;
		case NodeType::binary_op: result = "binary_op"; break;
		case NodeType::expression: result = "expression"; break;
		case NodeType::simple_expression: result = "simple_expression"; break;
		case NodeType::term: result = "term"; break;
		case NodeType::factor: result = "factor"; break;
		case NodeType::type: result = "type"; break;
		case NodeType::record_type: result = "record_type"; break;
		case NodeType::array_type: result = "array_type"; break;
		case NodeType::field_list: result = "field_list"; break;
		case NodeType::ident_list: result = "ident_list"; break;
		case NodeType::procedure_heading: result = "procedure_heading"; break;
		case NodeType::procedure_body: result = "procedure_body"; break;
		case NodeType::formal_parameters: result = "formal_parameters"; break;
		case NodeType::fp_section: result = "fp_section"; break;
		case NodeType::statement_sequence: result = "statement_sequence"; break;
		case NodeType::statement: result = "statement"; break;
		case NodeType::if_statement: result = "if_statement"; break;
		case NodeType::while_statement: result = "while_statement"; break;
		case NodeType::acutal_parameters: result = "acutal_parameters"; break;
		case NodeType::selector: result = "selector"; break;
		case NodeType::assignment: result = "assignment"; break;
		case NodeType::procedure_call: result = "procedure_call"; break;
	}
	stream << result;
	return stream;
}

void Node::print(std::ostream & stream) const {
	if (!this->value_.empty()) {
		stream << this->getNodeType() << ": " << this->getValue();
	} else {
		stream << this->getNodeType();
	}
}

void Node::printTree(std::ostream & stream) const
{
	printTreeRec(stream, 0);
}

void Node::printTreeRec(std::ostream & stream, int depth) const
{
	for (int i = 0; i < depth; i++) {
		stream << "|-";
	}

	this->print(stream);
	stream << std::endl;

	for (Node child : children_) {
		child.printTreeRec(stream, depth + 1);
	}
}

void Node::addChild(Node node)
{
	children_.push_back(node);
}

std::string Node::getValue() const
{
	return value_;
}

std::ostream & operator<<(std::ostream & stream, const Node & node)
{
	node.printTree(stream);

	return stream;
}

Node::~Node() = default;