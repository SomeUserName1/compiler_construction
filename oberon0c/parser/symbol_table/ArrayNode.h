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
    std::string _type;
    // Elements are stored as children

  public:
    ArrayNode(std::string name, int size, std::string type);
    const int getSize();
    const std::string getType();
  const std::vector<std::shared_ptr<DeclarationNode>> getValues();
  const std::shared_ptr<DeclarationNode> getValue(int position);
};

#endif //OBERON0C_ARRAYNODE_H