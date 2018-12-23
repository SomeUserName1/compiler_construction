/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_NumberNode_H
#define OBERON0C_NumberNode_H

#include "DeclarationNode.h"

class NumberNode : public DeclarationNode{
  private:
    std::string _type;
    int _value;
    bool _isConst;

  public:
    NumberNode(std::string name, int value, bool isConst);
    const std::string getType() const;
    const int getValue() const;
    void setValue(int new_val);
};

#endif //OBERON0C_NumberNode_H
