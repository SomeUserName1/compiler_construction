/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ARRAYNODE_H
#define OBERON0C_ARRAYNODE_H

#include "DeclarationNode.h"
#include "NumberNode.h"

class ArrayNode : public DeclarationNode {
  private:
    int _size;
    std::string _type;
    // Elements are stored as children

  public:
    ArrayNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children,
              std::shared_ptr<SymbolTable> sym_table, std::string name, int size, std::string type);
    const int getSize();
    const std::string getType();
  // cast the ASTNodes to the nodes of the according type; as we only deal with ints the cast is chilling
  const std::vector<std::shared_ptr<NumberNode>> getValues();
  const std::shared_ptr<NumberNode> getValue(int position);

    // No setters necessary as arrays are fixed size and type; values are altered over the concrete values
};

#endif //OBERON0C_ARRAYNODE_H