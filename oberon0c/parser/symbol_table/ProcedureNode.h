/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * for function declaration, heterogenous children; actually the same as record type in terms of the object but may be
 *      extended to have a return type and children are not required to have a concrete value,
 *      if they do it's the default value of the param
 */

#ifndef OBERON0C_PROCEDURENODE_H
#define OBERON0C_PROCEDURENODE_H

#include "DeclarationNode.h"
#include "SymbolScopeNode.h"
class ProcedureNode : public DeclarationNode {
private:
  // HEADING
  // params are stored as children
  std::shared_ptr<SymbolScopeNode> _scope;

public:
  ProcedureNode(std::string name, std::vector<std::shared_ptr<Node>> parameters, std::shared_ptr<SymbolScopeNode> scope);

};

#endif //OBERON0C_PROCEDURENODE_H
