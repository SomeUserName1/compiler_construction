/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_HIGHERORDERTYPENODE_H
#define OBERON0C_HIGHERORDERTYPENODE_H

#include "DeclarationNode.h"
class TypeNode : public DeclarationNode{
public:
  explicit TypeNode(std::string name, std::shared_ptr<Node> type);

};

#endif //OBERON0C_HIGHERORDERTYPENODE_H
