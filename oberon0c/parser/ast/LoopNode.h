/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_CONTROLFLOWNODE_H
#define OBERON0C_CONTROLFLOWNODE_H

#include <Node.h>
#include <symbol_table/BooleanNode.h>
class LoopNode : public Node {
public:
  LoopNode(std::shared_ptr<BooleanNode> condition, std::shared_ptr<Node> body);
};

#endif //OBERON0C_CONTROLFLOWNODE_H
