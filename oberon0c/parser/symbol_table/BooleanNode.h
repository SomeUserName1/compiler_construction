/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * Helper class to evaluate conditions
 */

#ifndef OBERON0C_BOOLEANNODE_H
#define OBERON0C_BOOLEANNODE_H

#include "DeclarationNode.h"
class BooleanNode : public DeclarationNode{
private:
  bool _value;

public:
  BooleanNode(std::string name, int value);
  const int getValue() const;
  void setValue(int new_val);
};

#endif //OBERON0C_BOOLEANNODE_H
