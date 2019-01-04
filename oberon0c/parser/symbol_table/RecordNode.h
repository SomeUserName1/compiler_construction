/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_RECORDNODE_H
#define OBERON0C_RECORDNODE_H

#include "DeclarationNode.h"
#include "SymbolScopeNode.h"
class RecordNode : public DeclarationNode {
private:
  // scope stored as first child

public:
  RecordNode(std::string name, DeclarationType decl_type, std::shared_ptr<SymbolScopeNode> scope);
  const std::shared_ptr<DeclarationNode> getField(const std::string &name);
  const int getFieldPos(const std::string &name);
  const std::shared_ptr<SymbolScopeNode> getScope() const;
  void setValue(const std::string &name, std::shared_ptr<DeclarationNode> node);
};

#endif //OBERON0C_RECORDNODE_H
