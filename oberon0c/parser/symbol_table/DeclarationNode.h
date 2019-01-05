/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_DECLARATIONNODE_H
#define OBERON0C_DECLARATIONNODE_H

#include <Node.h>
enum class DeclarationType {CONST, VAR, TYPE, PROCEDURE, SCOPE, OP};

std::ostream& operator<<(std::ostream &ostr, DeclarationType type);

class DeclarationNode : public Node {
  private:
    std::string _name;
    DeclarationType _declaration_type;
    std::string _type;


  public:
    DeclarationNode(std::string name, DeclarationType decl_type, std::string type);
    DeclarationNode(std::string name, DeclarationType decl_type, std::string type, std::vector<std::shared_ptr<Node>> children);
    DeclarationNode(DeclarationNode& node);
    const std::string getName() const;
    void setName(std::string &new_name);
    const DeclarationType getDeclType() const;
    const std::string getType() const;
    void print(std::ostream & stream) const override;
    void printTreeRec(std::ostream & stream, int depth) const override;
  friend std::ostream& operator<<(std::ostream &stream, const DeclarationNode &node);
};
#endif //OBERON0C_DECLARATIONNODE_H
