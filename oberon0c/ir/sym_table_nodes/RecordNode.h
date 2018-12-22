/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_RECORDNODE_H
#define OBERON0C_RECORDNODE_H

#include "DeclarationNode.h"
class RecordNode : DeclarationNode {
  private:
    int _no_fields;

  public:
    RecordNode(std::shared_ptr<ASTNode> parent,
    std::vector<std::shared_ptr<ASTNode>> children,
        std::shared_ptr<SymbolTable> sym_table, std::string name, int
               no_fields);
    const int getNoFields();
    const std::shared_ptr<DeclarationNode> getField(const std::string &name);
	const std::vector<std::shared_ptr<DeclarationNode>> getFields();
};

#endif //OBERON0C_RECORDNODE_H
