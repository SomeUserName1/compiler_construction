/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_NumberNode_H
#define OBERON0C_NumberNode_H

#include "DeclarationNode.h"
#include <ASTNode.h>

class NumberNode : public DeclarationNode{
  private:
    type _type;
    int _value;
    bool _isConst;

  public:
    NumberNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children,
              std::shared_ptr<SymbolTable> &sym_table, std::string &name, int value, bool isConst);
    const type getType() const;
    const int getValue() const;
    void setValue(int new_val);
};

#endif //OBERON0C_NumberNode_H
