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
    size_t _afterCount;
    size_t _modCount;

    void init();
    void gen(const std::shared_ptr<ASTNode> &node);
    void finish();

    const std::string add() const;
    const std::string sub() const;
    const std::string div() const;
    const std::string mul() const;
    const std::string mod();
    const std::string invert(const std::shared_ptr<ASTNode> &node) const;
    const std::string push_const(const std::shared_ptr<ASTNode>& node) const;
    const std::string push_var(const std::shared_ptr<ASTNode>& node);
    const std::string push_address(const std::shared_ptr<ASTNode>& node) const;
    const std::string assign(const std::shared_ptr<ASTNode>& node) const;

    const std::string check_stack_alignment(std::string str) const;
    const std::string print_debug(std::string str, const std::shared_ptr<ASTNode>& node, bool is_addr) const;
    const size_t getOffset(const std::shared_ptr<ASTNode>& node) const;

public:
    CodeGen(std::shared_ptr<SymbolTable> sym, std::shared_ptr<ASTNode> ast);

};


#endif //OBERON0C_CODEGEN_H
