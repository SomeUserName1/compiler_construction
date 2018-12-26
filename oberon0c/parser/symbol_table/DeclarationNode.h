/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H

#include <Node.h>
enum class DeclarationType {CONST, VAR, TYPE, PROCEDURE};

std::ostream& operator<<(std::ostream &ostr, DeclarationType type);

class DeclarationNode : public Node {
  private:
    std::string _name;
    DeclarationType _DeclarationType;


  public:
    DeclarationNode(std::string name, DeclarationType type);
    DeclarationNode(std::string name, DeclarationType type, std::vector<std::shared_ptr<Node>> children);
    const std::string getName() const;
    const DeclarationType getDeclType() const;
};
#endif //OBERON0C_DECLARATIONNODE_H
