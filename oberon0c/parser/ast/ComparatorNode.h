/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_COMPARATORNODE_H
#define OBERON0C_COMPARATORNODE_H

#include <symbol_table/BooleanNode.h>

class ComparatorNode : public BooleanNode {

public:
  explicit ComparatorNode(std::string operator_);
  void addChild(std::shared_ptr <Node> newChild) override;
};

#endif //OBERON0C_COMPARATORNODE_H
