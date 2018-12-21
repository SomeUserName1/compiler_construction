/**
 * @author fabian.klopfer@uni-konstanz.de
 */

#ifndef OBERON0C_ASTNODE_H
#define OBERON0C_ASTNODE_H

#include <vector>
#include <memory>
#include <ostream>

/**
 * Common super class for all nodes in the AST. Provides generic print function and getter/setter for child and parent
 * pointers.
 */
class ASTNode {
  private:
    std::shared_ptr<ASTNode> _parent;
    std::vector<std::shared_ptr<ASTNode>> _children;

  public:
    ASTNode(std::shared_ptr<ASTNode> parent, std::vector<std::shared_ptr<ASTNode>> children);
    void addChildren(std::vector<std::shared_ptr<ASTNode>> newChildren);
    const std::shared_ptr<ASTNode> getParent();
    const std::vector<std::shared_ptr<ASTNode>> getChildren();
    //virtual const std::ostream& operator<<(std::ostream &stream);
    //virtual const void print();

  template <class T> constexpr std::string_view type_name() {
    using namespace std;
#ifdef __clang__
    string_view p = __PRETTY_FUNCTION__;
    return string_view(p.data() + 34, p.size() - 34 - 1);
#elif defined(__GNUC__)
    string_view p = __PRETTY_FUNCTION__;
#  if __cplusplus < 201402
    return string_view(p.data() + 36, p.size() - 36 - 1);
#  else
    return string_view(p.data() + 49, p.find(';', 49) - 49);
#  endif
#elif defined(_MSC_VER)
    string_view p = __FUNCSIG__;
    return string_view(p.data() + 84, p.size() - 84 - 7);
#endif
  }
};

#endif //OBERON0C_ASTNODE_H
