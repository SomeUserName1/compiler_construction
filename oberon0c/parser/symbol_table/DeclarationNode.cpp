#include <utility>

#include "DeclarationNode.h"

DeclarationNode::DeclarationNode(std::string name, DeclarationType type) : _name(std::move(name)),
_DeclarationType(type) {}

DeclarationNode::DeclarationNode(std::string name, DeclarationType type, std::vector<std::shared_ptr<Node>> children)
    : Node(std::move(children)), _name(std::move(name)), _DeclarationType(type) {}

const std::string DeclarationNode::getName() const {
  return this->_name;
}

const DeclarationType DeclarationNode::getDeclType() const {
  return this->_DeclarationType;;
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
  }
  ostr << result;
  return ostr;
}