/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H

#include <utility>
#include <ASTNode.h>
#include <SymbolTable.h>

class DeclarationNode : public ASTNode {
  private:
    std::shared_ptr<SymbolTable> _sym_table;
    std::string _name;

  public:
    DeclarationNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children,
        std::shared_ptr<SymbolTable> sym_table, std::string name);
    const std::shared_ptr<SymbolTable> getSymbolTable() const;
    void setSymbolTable(std::shared_ptr<SymbolTable> sym_tab);
    const std::string getName() const;


};

#endif //OBERON0C_DECLARATIONNODE_H
