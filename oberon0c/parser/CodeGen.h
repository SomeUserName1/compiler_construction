//
// Created by fabian on 26.01.19.
//

#ifndef OBERON0C_CODEGEN_H
#define OBERON0C_CODEGEN_H


#include <ast/ASTNode.h>
#include <sstream>
#include <string>
#include <ios>
#include <st/SymbolTable.h>

class CodeGen {
private:
    std::unique_ptr<SymbolTable> _sym;
    std::unique_ptr<ASTNode> _ast;
    std::stringstream _result;

    const std::stringstream add() const;
    const std::stringstream sub() const;
    const std::stringstream div() const;
    const std::stringstream mul() const;
    const std::stringstream push_const(const std::shared_ptr<ASTNode>& node) const;
    const std::stringstream push_var(const std::shared_ptr<ASTNode>& node) const;
    const std::stringstream push_address(const std::shared_ptr<ASTNode>& node) const;
    const std::stringstream assign(const std::shared_ptr<ASTNode>& node) const;
    const std::stringstream init() const;

public:
    explicit CodeGen(std::unique_ptr<SymbolTable> sym, std::unique_ptr<ASTNode> ast);
    std::stringstream gen(const std::shared_ptr<ASTNode> &node) const;

};


#endif //OBERON0C_CODEGEN_H
