/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_HIGHERORDERTYPENODE_H
#define OBERON0C_HIGHERORDERTYPENODE_H

#include "DeclarationNode.h"
class TypeDefNode : DeclarationNode{
private:
  std::string _type;

public:
  explicit TypeDefNode(std::string name);
  const std::string getType();

};

#endif //OBERON0C_HIGHERORDERTYPENODE_H
