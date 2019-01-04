#include <utility>

#include "ProcedureNode.h"
ProcedureNode::ProcedureNode(std::string name, std::vector<std::shared_ptr<DeclarationNode>> parameters,
    std::shared_ptr<SymbolScopeNode> scope, std::shared_ptr<Node> ast)
  : DeclarationNode(std::move(name), DeclarationType::PROCEDURE, "ProcedureNode"), _scope(std::move(scope)),
    _ast(std::move(ast)) {
  this->setParams(std::move(parameters));
}

void ProcedureNode::setParams(std::vector<std::shared_ptr<DeclarationNode>> parameters) {
  for(auto &param : parameters) {
    if ((param->getDeclType() == DeclarationType::VAR && param->getType() != "ArrayNode"
    && param->getType() != "RecordNode") || (param->getDeclType() == DeclarationType::TYPE && (param->getType() == "ArrayNode"
        || param->getType() == "RecordNode"))) {
      this->addChild(param);
    }
  }
}

void ProcedureNode::print(std::ostream & stream) const {
  stream << this->getName() << ", " << this->getDeclType() << ", " << this->getType() ;
}

void ProcedureNode::printTree(std::ostream & stream) const {
  printTreeRec(stream, 0);
}

void ProcedureNode::printTreeRec(std::ostream & stream, int depth) const
{
  for (int i = 0; i < depth; i++) {
    stream << "|-";
  }

  this->print(stream);
  stream << std::endl;

  for (const auto &child : this->getChildren()) {
    dynamic_cast<DeclarationNode &>(*child).printTreeRec(stream, depth + 1);
  }
  this->_scope->printTreeRec(stream, depth+1);
  //this->_ast->printTreeRec(stream, depth+1)
}

std::ostream & operator<<(std::ostream & stream, const ProcedureNode & node) {
  node.printTree(stream);

  return stream;
}