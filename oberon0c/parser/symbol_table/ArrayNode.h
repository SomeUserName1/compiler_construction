/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * TODO check types when assigning/initializing an array
 */

#ifndef OBERON0C_ARRAYNODE_H
#define OBERON0C_ARRAYNODE_H

#include "DeclarationNode.h"

class ArrayNode : public DeclarationNode {
  private:
    int _size;
    std::shared_ptr<DeclarationNode> _array_type;
    // Elements are stored as children

  public:
    ArrayNode(std::string name, DeclarationType decl_type, int size, std::shared_ptr<DeclarationNode> type);
  void setValue(int position, std::shared_ptr<DeclarationNode> node);
    const int getSize();
    const std::shared_ptr<DeclarationNode>  getArrayType();
  const std::vector<std::shared_ptr<DeclarationNode>> getValues();
  const std::shared_ptr<DeclarationNode> getValue(int position);
};

#endif //OBERON0C_ARRAYNODE_H