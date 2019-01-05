/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * Helper class to evaluate conditions
 */

#ifndef OBERON0C_BOOLEANNODE_H
#define OBERON0C_BOOLEANNODE_H

#include <assert.h>
#include "NumberNode.h"
class BooleanNode : public NumberNode {
public:
  BooleanNode(std::string name, int value);
  explicit BooleanNode(std::string name);
  void setValue(int value) override;
};

#endif //OBERON0C_BOOLEANNODE_H
