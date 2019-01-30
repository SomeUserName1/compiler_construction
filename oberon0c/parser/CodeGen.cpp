#include "CodeGen.h"

CodeGen::CodeGen(const std::shared_ptr<SymbolTable> sym, std::shared_ptr<ASTNode> ast) : _sym(sym),
    _ast(std::move(ast)) {
    init();
    gen(this->_ast);
    finish();
}

void CodeGen::init() {
    size_t var_size = this->_sym->size();
    size_t var_s_stack_aligned = var_size + (16 - (var_size % 16));

    std::stringstream _asm;
    _asm << ";=====INIT======"                                                     << std::endl
         << "global   main"                                                        << std::endl
         << "extern   printf"                                                      << std::endl
                                                                                   << std::endl
         << "section  .data"                                                       << std::endl
         << "   amark:   db    \"Address => lhs of assignment\", 10, 0"            << std::endl
         << "   vmark:   db    \"Variable => rhs of assignment\", 10, 0"           << std::endl
         << "   amsg:    db    \"  The address is %x!\", 10, 0"                    << std::endl
         << "   avmsg:   db    \"The value at the assigned address is %d\", 10, 0" << std::endl
         << "   vmsg:    db    \"  [value] is %d!\", 10, 0"                        << std::endl
         << "   fail_su: db    \"Stack unaligned!\",10,0"                          << std::endl
                                                                                   << std::endl
         << "section .text"                                                        << std::endl
                                                                                   << std::endl
         << "main:"                                                                << std::endl
         << "   push    rbp"                                                       << std::endl
         << "   mov     rbp, rsp"                                                  << std::endl
         << "   ;reserve space for vars by moving the stack "                      << std::endl
         << "   ;pointer as many bytes as the maximum offset "                     << std::endl
         << "   ;in the symbol table"                                              << std::endl
         << "   sub    rsp," << var_s_stack_aligned                                << std::endl;


        for (int i = 0; i < var_s_stack_aligned; i=i+8) {
            _asm << "    mov    QWORD  [rsp+"<< i << "], 0"                        << std::endl;
        }
    this->_result = this->_result + _asm.str();
}

void CodeGen::finish() {
    std::ofstream myfile;
    myfile.open("out.s");
    // Insert generated asm
    myfile << this->_result                         << std::endl
           << "    xor    rax,rax"                  << std::endl
    // clean up and return
           << "    mov    rsp,rbp"                  << std::endl
           << "    pop    rbp"                      << std::endl
           << "    ret"                             << std::endl
                                                    << std::endl
           << ".fail_stack_unaligned:"              << std::endl
           << "    mov    rdi, fail_su"             << std::endl
           << "    call   printf"                   << std::endl
           << "    xor    rax,rax"                  << std::endl
           << "    mov    rsp,rbp"                  << std::endl
           << "    pop    rbp"                      << std::endl
           << "    ret"                             << std::endl

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
            break;
        }
        case ASTNodeType::_int_not: {
            gen(node->getChildren().front());
            this->_result = this->_result + invert(node);
            break;
        }
        default: break;
    }
    this->_result = this->_result +  "\n";
}

const std::string CodeGen::check_stack_alignment(std::string str) const  {
    std::stringstream _asm;
    _asm << str << std::endl
        << "    mov    rdx,0"                 << std::endl
        << "    mov    rax, rsp"              << std::endl
        << "    mov    r8,16"                 << std::endl
        << "    div    r8"                    << std::endl
        << "    cmp    rdx, 0x0"              << std::endl
        << "    jne    .fail_stack_unaligned" << std::endl;

     return _asm.str();
}

const std::string CodeGen::print_debug(std::string str,const std::shared_ptr<ASTNode>& node, bool is_addr) const {
    size_t padding_offset = getOffset(node);
    std::string msg = is_addr ? "    mov    rdi, amark" : "    mov    rdi, vmark";
    std::stringstream _asm;

   _asm << str << std::endl << msg                                                   << std::endl
                            << "    call   printf"                                   << std::endl
                            << "    mov    rdi, amsg"                                << std::endl
                            << "    mov    r8, rbp"                                  << std::endl
                            << "    sub    r8,"             << padding_offset        << std::endl
                            << "    mov    rsi, r8"                                  << std::endl
                            << "    call   printf"                                   << std::endl
                            << "    mov    rdi, vmsg"                                << std::endl
                            << "    mov    rsi, QWORD [rbp-"<< padding_offset << "]" << std::endl
                            << "    call   printf"                                   << std::endl;
   return _asm.str();
}

const std::string CodeGen::add() const {
    std::stringstream _asm;
    _asm << ";=====ADD======"   << std::endl
         << "    add    rsp, 8" << std::endl
         << "    pop    r9"     << std::endl
         << "    add    rsp, 8" << std::endl
         << "    pop    r8"     << std::endl
         << "    add    r8, r9" << std::endl
         << "    push   r8"     << std::endl
         << "    sub    rsp, 8" << std::endl;

    return check_stack_alignment(_asm.str());
}

const std::string CodeGen::sub() const {
    std::stringstream _asm;
    _asm << ";=====SUB======"    << std::endl
         << "    add    rsp, 8"  << std::endl
         << "    pop    r9"      << std::endl
         << "    add    rsp, 8"  << std::endl
         << "    pop    r8"      << std::endl
         << "    sub    r8, r9"  << std::endl
         << "    push   r8"      << std::endl
         << "    sub    rsp, 8"  << std::endl;

    return check_stack_alignment(_asm.str());
}

const std::string CodeGen::div() const {
    std::stringstream _asm;
    _asm << ";=====DIV======"     << std::endl
         << "    add    rsp, 8"   << std::endl
         << "    pop    r8"      << std::endl
         << "    add    rsp, 8"   << std::endl
         << "    pop    rax"       << std::endl
         << "    xor    rdx, rdx"       << std::endl
         << "    cqo"               << std::endl
         << "    idiv    r8"       << std::endl
         << "    push   rax"      << std::endl
         << "    sub    rsp, 8"   << std::endl;

    return check_stack_alignment(_asm.str());
}

const std::string CodeGen::mul() const {
    std::stringstream _asm;
    _asm << ";=====MUL======"   << std::endl
         << "    add    rsp, 8" << std::endl
         << "    pop    r8"    << std::endl
         << "    add    rsp, 8" << std::endl
         << "    pop    rax"     << std::endl
         << "    mul    r8"     << std::endl
         << "    push   rax"    << std::endl
         << "    sub    rsp, 8" << std::endl;

    return check_stack_alignment(_asm.str());
}

const std::string CodeGen::push_const(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH CONST======"                        << std::endl
         << "    push   " << node->getSymbol()->getValue()  << std::endl
         << "    sub    rsp, 8"                             << std::endl;

    return check_stack_alignment(_asm.str());
}

const std::string CodeGen::push_var(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH VAR======"                             << std::endl
         << "    push   QWORD [rbp-" << getOffset(node) << "]" << std::endl
         << "    sub    rsp, 8"                                << std::endl;

    return check_stack_alignment(_asm.str()); //print_debug(check_stack_alignment(_asm.str()), node, false);
}

const std::string CodeGen::push_address(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH ADDRESS======"              << std::endl
         << "    mov    r8,rbp"                     << std::endl
         << "    sub    r8,"     << getOffset(node) << std::endl
         << "    push   r8"                         << std::endl
         << "    sub    rsp, 8"                     << std::endl;

    return check_stack_alignment(_asm.str()) ; //print_debug(check_stack_alignment(_asm.str()), node, true);
}

const std::string CodeGen::assign(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====ASSIGN======"               << std::endl
         << "    add    rsp, 8"                << std::endl
         << "    pop    r9"                    << std::endl
         << "    add    rsp, 8"                << std::endl
         << "    pop    r8"                    << std::endl
         << "    mov    [r8], r9"              << std::endl
         << "    mov    rdi, avmsg"             << std::endl
         << "    mov    rsi, QWORD [r8]"    << std::endl
         << "    call   printf"                << std::endl;

    return check_stack_alignment(_asm.str());
}

const std::string  CodeGen::invert(const std::shared_ptr<ASTNode> &node) const {
    std::stringstream _asm;
    _asm << ";=====INVERT======" << std::endl;
    _asm << "   add    rsp, 8"   << std::endl;
    _asm << "   pop    r9"       << std::endl;
    _asm << "   not    r9"       << std::endl;
    _asm << "   inc    r9"       << std::endl;
    _asm << "   push   r9"       << std::endl;
    _asm << "   sub    rsp, 8"   << std::endl;

    return check_stack_alignment(_asm.str());
}

const size_t CodeGen::getOffset(const std::shared_ptr<ASTNode>& node) const {
    // +1 is as we dont want to overwrite the actual base pointer
    // +8 is as we are reading from low to high
    return  this->_sym->_offset(node->getSymbol())+9;
}
