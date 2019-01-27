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
#include <iostream>
#include <fstream>

class CodeGen {
private:
    std::shared_ptr<SymbolTable> _sym;
    std::shared_ptr<ASTNode> _ast;
    std::string _result;

    const std::string add() const;
    const std::string sub() const;
    const std::string div() const;
    const std::string mul() const;
    const std::string invert(const std::shared_ptr<ASTNode> &node) const;
    const std::string push_const(const std::shared_ptr<ASTNode>& node) const;
    const std::string push_var(const std::shared_ptr<ASTNode>& node) const;
    const std::string push_address(const std::shared_ptr<ASTNode>& node) const;
    const std::string assign(const std::shared_ptr<ASTNode>& node) const;
    void init();
    void finish();

public:
    CodeGen(std::shared_ptr<SymbolTable> sym, std::shared_ptr<ASTNode> ast);
    void gen(const std::shared_ptr<ASTNode> &node);

};


#endif //OBERON0C_CODEGEN_H
