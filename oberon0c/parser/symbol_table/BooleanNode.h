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
  const std::string _type = "BOOLEAN";
  bool _value;

public:
  BooleanNode(std::string name, int value);
  const std::string getType() const;
  const int getValue() const;
  void setValue(int new_val);
};

#endif //OBERON0C_BOOLEANNODE_H
