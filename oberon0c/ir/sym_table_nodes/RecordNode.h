/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_RECORDNODE_H
#define OBERON0C_RECORDNODE_H

#include "DeclarationNode.h"
class RecordNode : DeclarationNode {
  private:
    int no_fields;

  public:
    ArrayNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children,
        std::shared_ptr<SymbolTable> sym_table, std::string name, int size, std::string type);
    const int getNoFields();
    // Deal with all kinds of types (overloading or switch case?)
    const std::vector<std::shared_ptr<ASTNode>> getValues();
    const std::shared_ptr<ASTNode> getValue(int position);
};

#endif //OBERON0C_RECORDNODE_H
