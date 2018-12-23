#include "ProcedureNode.h"
ProcedureNode::ProcedureNode(std::string name, int no_params) : DeclarationNode(std::move(name)), _no_params(no_params){
  // TODO new Symbol table

}
const int ProcedureNode::getNoParams() {
  return this->_no_params;
}
