/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H

#include <Node.h>

class DeclarationNode : public Node {
  private:
    std::string _name;

  public:
    explicit DeclarationNode(std::string name);
    const std::string getName() const;
};
#endif //OBERON0C_DECLARATIONNODE_H
