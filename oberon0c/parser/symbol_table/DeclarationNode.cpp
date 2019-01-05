#include "DeclarationNode.h"

DeclarationNode::DeclarationNode(std::string name, DeclarationType decl_type, std::string type) : _name(std::move(name)),
_declaration_type(decl_type), _type(std::move(type)) {}

DeclarationNode::DeclarationNode(std::string name, DeclarationType decl_type, std::string type,
    std::vector<std::shared_ptr<Node>> children)
    : Node(std::move(children)), _name(std::move(name)), _declaration_type(decl_type), _type(std::move(type)) {}

const std::string DeclarationNode::getName() const {
  return this->_name;
}

const DeclarationType DeclarationNode::getDeclType() const {
  return this->_declaration_type;;
}
void DeclarationNode::setName(std::string &new_name) {
  this->_name = new_name;
}
DeclarationNode::DeclarationNode(DeclarationNode &node)
  : Node(node.getChildren()), _name(node.getName()), _declaration_type(node.getDeclType()), _type(node.getType()) {

}
const std::string DeclarationNode::getType() const {
  return _type;
}

std::ostream& operator<<(std::ostream &ostr, DeclarationType type) {
  std::string result;
  switch(type) {
  case DeclarationType::CONST: result = "CONST";
    break;
  case DeclarationType::VAR: result = "VAR";
    break;
  case DeclarationType::TYPE: result = "TYPE";
    break;
  case DeclarationType::PROCEDURE: result = "PROCEDURE";
    break;
  case DeclarationType::SCOPE: result = "SCOPE";
    break;
  case DeclarationType::OP: result = "OP";
    break;
  }
  ostr << result;
  return ostr;
}


void DeclarationNode::print(std::ostream & stream) const {
  stream << "|_" << this->_name << ", " << this->_declaration_type << ", " << this->_type ;
}

void DeclarationNode::printTreeRec(std::ostream & stream, int depth) const
{
  for (int i = 0; i < depth; i++) {
    stream << "  ";
  }

  this->print(stream);
  stream << std::endl;

  for (const auto &child : this->getChildren()) {
    dynamic_cast<DeclarationNode &>(*child).printTreeRec(stream, depth + 1);
  }
}

std::ostream & operator<<(std::ostream & stream, const DeclarationNode & node) {
  node.printTree(stream);

  return stream;
}