#include "DeclarationNode.h"

DeclarationNode::DeclarationNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children,
    const std::shared_ptr<SymbolTable> sym_table, const std::string name)
    : ASTNode(std::move(parent), std::move(children)), _sym_table(sym_table), _name(name) {}

const std::shared_ptr<SymbolTable> DeclarationNode::getSymbolTable() const {
  return std::shared_ptr<SymbolTable>();
}

void DeclarationNode::setSymbolTable(std::shared_ptr<SymbolTable> sym_tab) {
  this->_sym_table = std::move(sym_tab);
}
const std::string DeclarationNode::getName() const {
  return this->_name;
}
