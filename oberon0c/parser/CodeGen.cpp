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
 */

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
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
        }
        case ASTNodeType::plus: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            *_result << add().str();
        }
        case ASTNodeType ::minus: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            *_result << sub().str();
        }
        case ASTNodeType::div: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            *_result << div().str();
        }
        case ASTNodeType::times: {
            for (const auto &child : node->getChildren()) {
                gen(child);
            }
            *_result << mul().str();
        }
        case ASTNodeType::assignment: {
            for (const auto &child : node->getChildren()) {
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

