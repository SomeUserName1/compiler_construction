#include "TypeDefNode.h"
TypeDefNode::TypeDefNode(std::string name) : DeclarationNode(std::move(name)) {

}
const std::string TypeDefNode::getType() {
  return this->_type;
}

