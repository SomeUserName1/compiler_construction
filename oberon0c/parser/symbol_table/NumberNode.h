/**
 * @author fabian.klopfer@uni-konstanz.de
 *
 * always a variable (regardless of a constant or non constant one
 */

#ifndef OBERON0C_NumberNode_H
#define OBERON0C_NumberNode_H

#include "DeclarationNode.h"

class NumberNode : public DeclarationNode {
  private:
    int _value;
    bool _init = false;

  public:
    NumberNode(std::string name, int value, DeclarationType declarationType);
    NumberNode(std::string, DeclarationType declarationType);
    NumberNode(std::string, DeclarationType declarationType, std::string type);
    const int getValue() const;
    virtual void setValue(int new_val);
    void print(std::ostream & stream) const override;
    void printTreeRec(std::ostream & stream, int depth) const override;
    friend std::ostream& operator<<(std::ostream &stream, const NumberNode &node);
};

#endif //OBERON0C_NumberNode_H
