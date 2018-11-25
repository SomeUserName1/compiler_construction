#include "Node.h"

Node::Node(NodeType nodeType, FilePos pos)
{
	nodeType_ = nodeType;
	pos_ = pos;
	value_ = nullptr;
}

Node::Node(NodeType nodeType, FilePos pos, std::string value)
{
	nodeType_ = nodeType;
	pos_ = pos;
	value_ = value;
}

const NodeType Node::getNodeType() const
{
	return nodeType_;
}

const FilePos Node::getFilePos() const
{
	return pos_;
}

void Node::print(std::ostream & stream)
{
	stream << this;
}

void Node::addChild(Node node)
{
	childs_.push_back(node);
}

std::string Node::getValue() const
{
	return value_;
}

std::ostream & operator<<(std::ostream & stream, const Node & node)
{
	stream << (char) node.getNodeType;
}

Node::~Node() {
}