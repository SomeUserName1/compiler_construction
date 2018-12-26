/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * 1. Symbols
 *      1.1 Modules, Procedures, Records define new scopes
 *      1.2 all names in the same scope must be unique
 */

#ifndef OBERON0C_SYMBOLTABLE_H
#define OBERON0C_SYMBOLTABLE_H

#include <Node.h>
#include "DeclarationNode.h"

class SymbolScopeNode : public Node {
private:
  std::shared_ptr<SymbolScopeNode> _parent;
  std::shared_ptr<Logger> _logger;

public:
  SymbolScopeNode(std::shared_ptr<SymbolScopeNode> parent, std::shared_ptr<Logger> logger);
  std::shared_ptr<SymbolScopeNode> getContainingScope();
  void addChild(std::initializer_list<std::shared_ptr<Node>> newChild) override;
  void addChild(std::shared_ptr<Node> newChild) override;
  bool checkCollision(std::shared_ptr<Node> new_child);
  void symbolCollision(std::shared_ptr<DeclarationNode> new_child, std::shared_ptr<DeclarationNode> element);
  // TODO Symbol Table
};


#endif //OBERON0C_SYMBOLTABLE_H
