/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_BOOLEANOPNODE_H
#define OBERON0C_BOOLEANOPNODE_H

#include <Node.h>
#include <symbol_table/BooleanNode.h>
#include "ComparatorNode.h"
class BooleanOpNode : public BooleanNode {
private:

public:
  explicit BooleanOpNode(std::string operator_);
  void addChild(std::shared_ptr <Node> newChild) override;
};

#endif //OBERON0C_BOOLEANOPNODE_H
