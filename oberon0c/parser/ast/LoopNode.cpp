#include <utility>

#include "LoopNode.h"
LoopNode::LoopNode(std::shared_ptr<BooleanNode> condition, std::shared_ptr<Node> body) {
    this->addChild(std::move(condition));
    this->addChild(std::move(body));
}
