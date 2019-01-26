//
// Created by fabian on 26.01.19.
//

#include "CodeGen.h"

CodeGen::CodeGen(std::unique_ptr<SymbolTable> sym, std::unique_ptr<ASTNode> ast) : _sym(std::move(sym)),
    _ast(std::move(ast)) {
    this->_result;
    init();
}

const std::stringstream CodeGen::init() const {

    return std::stringstream();
}

std::stringstream CodeGen::gen(const std::shared_ptr<ASTNode> &node) const {
    std::stringstream generated;
    generated << std::endl;
    switch(node->getNodeType()) {
        case ASTNodeType::statement_sequence: {
            for (auto child : node->getChildren()) {
                this->_result << gen(std::make_shared<ASTNode>(child)).str();
            }
        }
        case ASTNodeType::plus: {
            for (auto child : node->getChildren()) {
                this->_result <<  gen(std::make_shared<ASTNode>(child)).str();
            }
            this->_result << add().str();
        }
        case ASTNodeType ::minus: {
            for (auto child : node->getChildren()) {
                this->_result << gen(std::make_shared<ASTNode>(child));
            }
            this->_result << sub();
        }
        case ASTNodeType::div: {
            for (auto child : node->getChildren()) {
                this->_result << gen(std::make_shared<ASTNode>(child));
            }
            this->_result << div();
        }
        case ASTNodeType::times: {
            for (auto child : node->getChildren()) {
                this->_result << gen(std::make_shared<ASTNode>(child));
            }
            this->_result << mul();
        }
        case ASTNodeType::assignment: {
            for (auto child : node->getChildren()) {
                this->_result << gen(std::make_shared<ASTNode>(child));
            }
            this->_result << assign(node);
        }
        case ASTNodeType::_constant: {
            this->_result << push_const(node);
        }
        case ASTNodeType ::_deref: {
            this->_result << push_var(node);
        }
        case ASTNodeType::_addr: {
            this->_result << push_address(node);
        }
        case ASTNodeType::_not_int: {
            // TODO GenCode that inverts an int (shift and toggle)
        }
        default: break;
    }
    this->_result <<  generated.str();
    return this-> _result;
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

