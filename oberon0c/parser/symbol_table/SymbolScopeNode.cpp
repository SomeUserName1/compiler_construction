#include <utility>

#include "SymbolScopeNode.h"

SymbolScopeNode::SymbolScopeNode(std::shared_ptr<SymbolScopeNode> parent, std::shared_ptr<Logger> logger)
  : _parent(std::move(parent)), _logger(std::move(logger)) {}

void SymbolScopeNode::addChild(std::shared_ptr<Node> new_child) {
  checkCollision(new_child);
  Node::addChild(new_child);
}

void SymbolScopeNode::addChild(std::initializer_list<std::shared_ptr<Node>> newChild) {
  for (auto& elem : newChild) {
    checkCollision(elem);
  }
  Node::addChild(newChild);
}

std::shared_ptr<SymbolScopeNode> SymbolScopeNode::getContainingScope() {
  return this->_parent;
}

bool SymbolScopeNode::checkCollision(std::shared_ptr<Node> new_child) {
  auto new_decl_node = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*new_child));
  auto name = new_decl_node->getName();
  for (auto &elem : this->getChildren()) {
    auto decl_elem = std::make_shared<DeclarationNode>(dynamic_cast<DeclarationNode &>(*elem));
    if (decl_elem->getName() == name) {
      symbolCollision(new_decl_node, decl_elem);
      return false;
    }
  }
  return true;
}

void SymbolScopeNode::symbolCollision(std::shared_ptr<DeclarationNode> new_child,
                                      std::shared_ptr<DeclarationNode> element) {
    std::stringstream ss;
    ss << "Symbol Collision: Between the new node " << new_child->getName() << " declaring a " << new_child->getDeclType()
    << " and " << element->getName() << "declaring a " << element->getDeclType() << std::endl;
    std::string msg = ss.str();
    this->_logger->error(msg);
}

