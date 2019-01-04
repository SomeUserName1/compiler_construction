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
  // HEAD
  // params are stored as children

  // BODY
  std::shared_ptr<SymbolScopeNode> _scope;
  std::shared_ptr<Node> _ast;

public:
  ProcedureNode(std::string name, std::vector<std::shared_ptr<DeclarationNode>> parameters,
      std::shared_ptr<SymbolScopeNode> scope, std::shared_ptr<Node> ast);
  void setParams(std::vector<std::shared_ptr<DeclarationNode>> parameters);
  void print(std::ostream & stream) const override;
  void printTree(std::ostream & stream) const override;
  void printTreeRec(std::ostream & stream, int depth) const override;

};

#endif //OBERON0C_PROCEDURENODE_H
