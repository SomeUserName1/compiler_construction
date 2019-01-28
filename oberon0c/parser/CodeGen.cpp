#include "CodeGen.h"

CodeGen::CodeGen(const std::shared_ptr<SymbolTable> sym, std::shared_ptr<ASTNode> ast) : _sym(sym),
    _ast(std::move(ast)) {
    init();
    gen(this->_ast);
    finish();
}

void CodeGen::init() {
    std::stringstream _asm;
    _asm << ";=====INIT======"                                  << std::endl
         << "global   main"                                     << std::endl
         << "extern   printf"                                   << std::endl
                                                                << std::endl
         << "section  .data"                                    << std::endl
         << "   msg:     db       \"The value is %d!\", 10, 0 " << std::endl
                                                                << std::endl
         << "section .text"                                     << std::endl
                                                                << std::endl
         << "main:"                                             << std::endl
         << "   push    rbp"                                    << std::endl
         << "   mov     rbp, rsp"                               << std::endl
         << "   ;reserve space for vars by moving the stack "   << std::endl
         << "   ;pointer as many bytes as the maximum offset "  << std::endl
         << "   ;in the symbol table"                           << std::endl
         << "   sub    rsp,"
            << this->_sym->size() + (16 - (this->_sym->size() % 16))
                                                                << std::endl;

    this->_result = this->_result + _asm.str();
}

void CodeGen::finish() {
    std::ofstream myfile;
    myfile.open("out.s");
    // Insert generated asm
    myfile << this->_result                          << std::endl
    // print 42 as example
    // TODO print x,y,z or remember the assigned vars and print at the end
           << "     mov      rdi, msg"               << std::endl
           << "mov      r8,rbp"                      << std::endl
           << "add      r8, 8"                       << std::endl
           << "mov      rsi, r8"                     << std::endl
           << "     call     printf"                 << std::endl
           << "     xor      rax,rax"                << std::endl
    // clean up and return
           << "     mov      rsp,rbp"                << std::endl
           << "     pop      rbp"                    << std::endl
           << "     ret"                             << std::endl
           << "\n\n\n"
    // Append binary compilation and linking commands as comments
           << ";nasm -felf64 -o out.o out.s"        << std::endl
           << ";clang -o out out.o"                 << std::endl;
    myfile.close();
}

void CodeGen::gen(const std::shared_ptr<ASTNode> &node) {
    switch(node->getNodeType()) {
        case ASTNodeType::statement_sequence: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            break;
        }
        case ASTNodeType::plus: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            this->_result = this->_result + add();
            break;
        }
        case ASTNodeType ::minus: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            this->_result = this->_result + sub();
            break;
        }
        case ASTNodeType::div: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            this->_result = this->_result + div();
            break;
        }
        case ASTNodeType::times: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            this->_result = this->_result + mul();
            break;
        }
        case ASTNodeType::assignment: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            this->_result = this->_result + assign(node);
            break;
        }
        case ASTNodeType::_constant: {
            this->_result = this->_result + push_const(node);
            break;
        }
        case ASTNodeType ::_deref: {
            this->_result = this->_result + push_var(node);
            break;
        }
        case ASTNodeType::_addr: {
            this->_result = this->_result + push_address(node);
            // TODO Print vars that got assigned to
            break;
        }
        case ASTNodeType::_int_not: {
            this->_result = this->_result + invert(node);

            gen(node->getChildren()[0]);
            break;
        }
        default: break;
    }
    this->_result = this->_result +  "\n";
}

const std::string CodeGen::add() const {
    std::stringstream _asm;
    _asm << ";=====ADD======" << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r9"      << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r8"      << std::endl;
    _asm << "   add    r8, r9"  << std::endl;
    _asm << "   push   r8"      << std::endl;
    _asm << "   sub    rsp, 8" << std::endl;

    return _asm.str();
}

const std::string CodeGen::sub() const {
    std::stringstream _asm;
    _asm << ";=====SUB======" << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r9"      << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r8"      << std::endl;
    _asm << "   sub    r8, r9"  << std::endl;
    _asm << "   push   r8"      << std::endl;
    _asm << "   sub    rsp, 8" << std::endl;

    return _asm.str();
}

const std::string CodeGen::div() const {
    std::stringstream _asm;
    _asm << ";=====DIV======" << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r9"      << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r8"      << std::endl;
    _asm << "   div    r9"      << std::endl;
    _asm << "   push   r8"      << std::endl;
    _asm << "   sub    rsp, 8" << std::endl;
    // TODO: div funktioniert so nicht, siehe https://www.cs.uaf.edu/2017/fall/cs301/reference/x86_64.html unter
    //  Instructions
    return _asm.str();
}

const std::string CodeGen::mul() const {
    std::stringstream _asm;
    _asm << ";=====MUL======" << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r9"      << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r8"      << std::endl;
    _asm << "   mul    r9"      << std::endl;
    _asm << "   push   r8"      << std::endl;
    _asm << "   sub    rsp, 8" << std::endl;
    // TODO: mul funktioniert so ebenfalls nicht, siehe gleiche Quelle.

    return _asm.str();
}

const std::string CodeGen::push_const(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH CONST======" << std::endl;
    _asm << "   push   " << node->getSymbol()->getValue()     << std::endl;
    _asm << "   sub    rsp, 8"        << std::endl;

    return _asm.str();
}

const std::string CodeGen::push_var(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH VAR======" << std::endl;
    _asm << "   push   QWORD [rbp-" << this->_sym->_offset(node->getSymbol()) <<"]" << std::endl;
    _asm << "   sub    rsp, 8"                                                    << std::endl;

    return _asm.str();
}

const std::string CodeGen::push_address(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH ADDRESS======" << std::endl;
    _asm << "   mov    r8,rbp"                                             << std::endl;
    _asm << "; offset is " << this->_sym->_offset(node->getSymbol()) << std::endl;
    _asm << "   sub    r8," << this->_sym->_offset(node->getSymbol())      << std::endl;
    _asm << "   push   r8"                                                 << std::endl;
    _asm << "   sub    rsp, 8"                                            << std::endl;
    return _asm.str();
}

const std::string CodeGen::assign(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====ASSIGN======" << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r9"      << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r8"      << std::endl;
    _asm << "   mov    [r8], r9" << std::endl;

    return _asm.str();
}

const std::string  CodeGen::invert(const std::shared_ptr<ASTNode> &node) const {
    std::stringstream _asm;
    _asm << ";=====INVERT======" << std::endl;
    _asm << "   add    rsp, 8" << std::endl;
    _asm << "   pop    r9"      << std::endl;
    _asm << "   not    r9"      << std::endl;
    _asm << "   inc    r9"      << std::endl;
    _asm << "   push   r9"      << std::endl;
    _asm << "   sub    rsp, 8" << std::endl;

    return _asm.str();
}

