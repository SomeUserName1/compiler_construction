/**
 * Lieber Daniel,
 *
 * solltest du mit deiner Freizeit nichts besseres anfangen zu wissen als meinen _UNFERTIGEN_ Code zu lesen und diesen
 * zu verbessern noch bevor ich ihn aber auch nur das erste Mal ausgeführt habe, kannst du ihn ja auch einfach direkt
 * fertig oder selbst ("from scratch") schreiben.
 * Ebenfalls ist es äusserst unangebracht mich am Sontag zu fragen ob ich denn auch arbeite. Ich denke ich bin alt genug
 * mir die Arbeitszeit selbst einzuteilen und bisher hat es jedes mal so gut geklappt, dass ich sowohl den Schein,
 * wie auch die Klausur absolviert habe.
 *
 * Ich bin versucht deinen Code ebenfalls zu kommentieren, wo unfertig fehlerhaft oder nicht nach C++ Idiomen
 * geschrieben.
 * Das lass ich allerdings, da ich denke dass du das 1. selbst weißt und 2. mir egal ist was du fabrizierst. Wenn du 9
 * Tage vor der Abgabe so dringend fertig werden willst damit du lernen kannst, wieso nicht einfach lernen statt hier
 * deine Schokoladenseite zu zeigen
 *
 * Viele Grüße,
 * Der auf den du dich nicht verlassen kannst ;)
 *
 * P.S.
 * Statt der schlauen Kommentare über den x86 Spaß den ich nicht auswendig kann und so wie du später nachgeschaut hätte
 * WENN DER REST LÄUFT OHNE SIGSEGV, hätten dir auch die fehlenden breaks auffallen können - der Grund für die SegV.
 * Ansonsten gäbe es auch so noch 1-2 Dinge zu tun:
 *
 * =================================================================
==14666==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 128 byte(s) in 1 object(s) allocated from:
    #0 0x515db0 in operator new(unsigned long) (/home/someusername/snap/nextcloud-client/10/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/cmake-build-debug/oberon0c+0x515db0)
    #1 0x52fde5 in Parser::ident() /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/parser/Parser.cpp:63:24
    #2 0x52eb7e in Parser::module() /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/parser/Parser.cpp:102:32
    #3 0x52db09 in Parser::parse() /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/parser/Parser.cpp:26:31
    #4 0x5ad8ae in main /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/main.cpp:28:26
    #5 0x7faf123bdb96 in __libc_start_main /build/glibc-OTsEL5/glibc-2.27/csu/../csu/libc-start.c:310

Indirect leak of 93 byte(s) in 1 object(s) allocated from:
    #0 0x515db0 in operator new(unsigned long) (/home/someusername/snap/nextcloud-client/10/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/cmake-build-debug/oberon0c+0x515db0)
    #1 0x51ceab in __gnu_cxx::new_allocator<char>::allocate(unsigned long, void const*) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/ext/new_allocator.h:111:27
    #2 0x51cdeb in std::allocator_traits<std::allocator<char> >::allocate(std::allocator<char>&, unsigned long) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/alloc_traits.h:436:20
    #3 0x51ca38 in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_create(unsigned long&, unsigned long) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/basic_string.tcc:153:14
    #4 0x51c475 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char*>(char*, char*, std::forward_iterator_tag) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/basic_string.tcc:219:14
    #5 0x51c270 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct_aux<char*>(char*, char*, std::__false_type) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/basic_string.h:236:11
    #6 0x51c014 in void std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::_M_construct<char*>(char*, char*) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/basic_string.h:255:4
    #7 0x51be29 in std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >::basic_string(std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> > const&) /usr/bin/../lib/gcc/x86_64-linux-gnu/7.3.0/../../../../include/c++/7.3.0/bits/basic_string.h:440:9
    #8 0x51b9ae in FilePos::FilePos(FilePos const&) /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/scanner/../util/Logger.h:13:8
    #9 0x51aeff in Token::getPosition() const /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/scanner/Token.cpp:19:12
    #10 0x52fe0f in Parser::ident() /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/parser/Parser.cpp:63:61
    #11 0x52eb7e in Parser::module() /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/parser/Parser.cpp:102:32
    #12 0x52db09 in Parser::parse() /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/parser/Parser.cpp:26:31
    #13 0x5ad8ae in main /home/someusername/Nextcloud/workspace/uni/7/compiler_construction/oberon0c/main.cpp:28:26
    #14 0x7faf123bdb96 in __libc_start_main /build/glibc-OTsEL5/glibc-2.27/csu/../csu/libc-start.c:310

SUMMARY: AddressSanitizer: 221 byte(s) leaked in 2 allocation(s).
 */

#include "CodeGen.h"

CodeGen::CodeGen(const std::shared_ptr<SymbolTable> sym, std::shared_ptr<ASTNode> ast) : _sym(sym),
    _ast(std::move(ast)) {
    init();
    gen(this->_ast);
    finish();
}

void CodeGen::init() {
    std::stringstream _asm;
    _asm << ";=====INIT======"                                  << std::endl;
    _asm << "global   main"                                     << std::endl;
    _asm << "extern   printf"                                   << std::endl;
    _asm << std::endl;
    _asm << "section  .data"                                    << std::endl;
    _asm << "   msg:     db       \"The value is %d!\", 10, 0 " << std::endl;
    _asm << std::endl;
    _asm << "section .text"                                     << std::endl;
    _asm << std::endl;
    _asm << "main:"                                     << std::endl;


    this->_result = this->_result + _asm.str();
}

void CodeGen::finish() {
    // TODO prints

    this->_result = this->_result + ";nasm -felf64 -o out.o out.asm\n "
                                    ";clang -o out out.o\n";

    std::ofstream myfile;
    myfile.open("out.asm");
    myfile << this->_result;
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
    _asm << "   push   QWORD [rbp-" << this->_sym->_offset(node->getSymbol()) <<"]"      << std::endl;
    _asm << "   sub    rsp, 8"                                                    << std::endl;

    return _asm.str();
}

const std::string CodeGen::push_address(const std::shared_ptr<ASTNode>& node) const {
    std::stringstream _asm;
    _asm << ";=====PUSH ADDRESS======" << std::endl;
    _asm << "   mov    r8,rbp"                                             << std::endl;
    _asm << "   sub    r8," << this->_sym->_offset(node->getSymbol())      << std::endl;
    _asm << "   push   r8"                                                 << std::endl;
    _asm << "   sub    rsp, 8"                                            << std::endl;
    // TODO: add  push_var  minus?
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

