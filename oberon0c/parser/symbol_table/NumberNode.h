/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * always a variable (regardless of a constant or non constant one
 */

#ifndef OBERON0C_NumberNode_H
#define OBERON0C_NumberNode_H

#include "DeclarationNode.h"

class NumberNode : public DeclarationNode {
  private:
    int _value;

  public:
    NumberNode(std::string name, int value, DeclarationType declarationType);
    NumberNode(std::string, DeclarationType declarationType);
    const int getValue() const;
    void setValue(int new_val);
};

#endif //OBERON0C_NumberNode_H
