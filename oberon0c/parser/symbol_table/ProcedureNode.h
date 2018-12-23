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
class ProcedureNode : DeclarationNode {
private:
  int _no_params;
  // params are stored as children

public:
  ProcedureNode(std::string name, int no_params);
  const int getNoParams();
};

#endif //OBERON0C_PROCEDURENODE_H
