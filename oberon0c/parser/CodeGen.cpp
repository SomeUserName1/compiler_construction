//
// Created by fabian on 26.01.19.
//

#include "CodeGen.h"

CodeGen::CodeGen(const std::shared_ptr<SymbolTable> sym, std::shared_ptr<ASTNode> ast) : _sym(std::move(sym)),
    _ast(std::move(ast)) {
    init();
    gen(this->_ast);
    finish();
}

const std::stringstream CodeGen::init() const {

    return std::stringstream();
}

void CodeGen::finish() {
    // TODO prints
    std::ofstream myfile;
    myfile.open ("out.nasm");
    myfile << _result;
    myfile.close();
}

void CodeGen::gen(const std::shared_ptr<ASTNode> &node) {
    switch(node->getNodeType()) {
        case ASTNodeType::statement_sequence: {
            for (auto child : node->getChildren()) {
                gen(child);
            }
        }
        case ASTNodeType::plus: {
            for (auto child : node->getChildren()) {
                gen(child);
            }
            *_result << add().str();
        }
        case ASTNodeType ::minus: {
            for (auto child : node->getChildren()) {
                gen(child);
            }
            *_result << sub().str();
        }
        case ASTNodeType::div: {
            for (auto child : node->getChildren()) {
                gen(child);
            }
            *_result << div().str();
        }
        case ASTNodeType::times: {
            for (auto child : node->getChildren()) {
                gen(child);
            }
            *_result << mul().str();
        }
        case ASTNodeType::assignment: {
            for (auto child : node->getChildren()) {
                gen(child);
            }
            *_result << assign(node).str();
        }
        case ASTNodeType::_constant: {
            *_result << push_const(node).str();
        }
        case ASTNodeType ::_deref: {
            *_result << push_var(node).str();
        }
        case ASTNodeType::_addr: {
            *_result << push_address(node).str();
        }
        case ASTNodeType::_int_not: {
            // TODO GenCode that inverts an int (shift and toggle)
            *_result << invert(node).str();
            gen(node);
        }
        default: break;
    }
    *_result << std::endl;
}

const std::stringstream CodeGen::add() const {
    std::stringstream _asm;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r9"      << std::endl;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r8"      << std::endl;
    _asm << "add    r8, r9"  << std::endl;
    _asm << "push   r8"      << std::endl;
    _asm << "sub    rsp, 12" << std::endl;

    return _asm;
}

const std::stringstream CodeGen::sub() const {
    std::stringstream _asm;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r9"      << std::endl;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r8"      << std::endl;
    _asm << "sub    r8, r9"  << std::endl;
    _asm << "push   r8"      << std::endl;
    _asm << "sub    rsp, 12" << std::endl;

    return _asm;
}

const std::stringstream CodeGen::div() const {
    std::stringstream _asm;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r9"      << std::endl;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r8"      << std::endl;
    _asm << "div    r9"      << std::endl;
    _asm << "push   r8"      << std::endl;
    _asm << "sub    rsp, 12" << std::endl;
    // TODO: div funktioniert so nicht, siehe https://www.cs.uaf.edu/2017/fall/cs301/reference/x86_64.html unter
    //  Instructions
    return _asm;
}

const std::stringstream CodeGen::mul() const {
    std::stringstream _asm;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r9"      << std::endl;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r8"      << std::endl;
    _asm << "mul    r9"      << std::endl;
    _asm << "push   r8"      << std::endl;
    _asm << "sub    rsp, 12" << std::endl;
    // TODO: mul funktioniert so ebenfalls nicht, siehe gleiche Quelle.

    return _asm;
}

const std::stringstream CodeGen::push_const(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << "push   " << node->getSymbol()->getValue()     << std::endl;
    _asm << "sub    rsp, 12"        << std::endl;

    return _asm;
}

const std::stringstream CodeGen::push_var(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << "push   [rbp-" << this->_sym->_offset(node->getSymbol()) <<"]"      << std::endl;
    _asm << "sub    rsp, 12"                                                    << std::endl;

    return _asm;
}

const std::stringstream CodeGen::push_address(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << "mov    r8,rbp"                                             << std::endl;
    _asm << "add    r8," << this->_sym->_offset(node->getSymbol())      << std::endl;
    _asm << "push   r8"                                                 << std::endl;
    _asm << "sub    rsp, 12"                                            << std::endl;
    // TODO: Sicher das du hier add verwenden willst, obwohl du bei push_var mit minus arbeitest?
    return _asm;
}

const std::stringstream CodeGen::assign(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r9"      << std::endl;
    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r8"      << std::endl;
    _asm << "mov    [r8], r9" << std::endl;

    return _asm;
}

const std::stringstream  CodeGen::invert(const std::shared_ptr<ASTNode> &node) const {
    std::stringstream _asm;

    _asm << "add    rsp, 12" << std::endl;
    _asm << "pop    r9"      << std::endl;
    _asm << "not    r9"      << std::endl;
    _asm << "inc    r9"      << std::endl;
    _asm << "push   r9"      << std::endl;
    _asm << "sub    rsp, 12" << std::endl;

    return _asm;
}

