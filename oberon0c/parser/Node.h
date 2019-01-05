/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ASTNODE_H
#define OBERON0C_ASTNODE_H

#include <vector>
#include <memory>
#include <ostream>
#include <utility>
#include <string>
#include <ostream>

#include <Logger.h>

/**
 * Common super class for all nodes in the AST.
 */
class Node {
  private:
    std::vector<std::shared_ptr<Node>> _children;

  protected:
  explicit Node(std::vector<std::shared_ptr<Node>> children);

  public:
    Node();
    virtual ~Node();
    virtual void addChild(std::initializer_list<std::shared_ptr<Node>> newChild);
    virtual void addChild(std::shared_ptr<Node> newChild);
    virtual void setChild(int position, std::shared_ptr<Node> newChild);
    const std::vector<std::shared_ptr<Node>> getChildren() const;
    virtual void print(std::ostream & stream) const;
    virtual void printTree(std::ostream & stream) const;
    virtual void printTreeRec(std::ostream & stream, int depth) const;
    friend std::ostream& operator<<(std::ostream &stream, const Node &node);
};

#endif //OBERON0C_ASTNODE_H
