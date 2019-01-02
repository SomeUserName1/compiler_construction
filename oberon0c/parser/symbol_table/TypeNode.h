/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_HIGHERORDERTYPENODE_H
#define OBERON0C_HIGHERORDERTYPENODE_H

#include "DeclarationNode.h"
class TypeNode : public DeclarationNode{
private:
  std::string _type;
public:
  TypeNode(std::string name, std::shared_ptr<DeclarationNode> aliased, DeclarationType decl_type, std::string aliased_type);
  std::shared_ptr<DeclarationNode> getAliased();
  std::string getAliasedString();
  const void setAliasedValue(std::shared_ptr<DeclarationNode> node);
};

#endif //OBERON0C_HIGHERORDERTYPENODE_H
