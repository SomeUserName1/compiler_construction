#include "ProcedureNode.h"
ProcedureNode::ProcedureNode(std::string name, std::vector<std::shared_ptr<Node>> parameters, std::shared_ptr<SymbolScopeNode> scope)
  : DeclarationNode(std::move(name), DeclarationType::PROCEDURE, std::move(parameters)), _scope(std::move(scope)) {}
