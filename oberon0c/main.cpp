/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "codegen/CodeGen.h"

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }

    // Preparing
    std::string filename = argv[1];
    auto logger = std::make_unique<Logger>();
    logger->setLevel(LogLevel::DEBUG);

    // Scanning
    auto scanner = std::make_unique<Scanner>(filename, logger.get());

    // Parsing
    auto parser = std::make_unique<Parser>(scanner.get(), logger.get());
    auto parse = parser->parse();
    logger->info(filename, "Parsing complete.");

    // Code generation
    auto mainName = parse->getChildren().at(0)->getValue();
    auto mainCode = parse->getSymbolTable()->getSymbol(&mainName)->getAst();

    if (mainCode == nullptr) {
        logger->error(argv[1], "No main method found");
        return 1;
    }

    auto codeGen = std::make_shared<CodeGen>(parse->getSymbolTable(), mainCode);

    logger->info(filename, "Assembly generation complete.");

    char const *n_args[] = {"nasm", "-felf64", "-o", "out.o", "out.asm", nullptr};

    pid_t nasm_pid = fork();

    if (nasm_pid == 0) {
        execvp(n_args[0], const_cast<char* const*>(n_args));
    }

    int status;
    do {
        wait(&status);
        if(status == -1 && errno != ECHILD) {
            perror("Error during wait()");
            abort();
        }
    } while (status > 0);

    logger->info(filename, "Assembling complete.");

    char const *c_args[] = {"clang", "-o", "out", "out.o", nullptr};

    pid_t clang_pid = fork();

    if (clang_pid == 0) {
        execvp(c_args[0], const_cast<char* const*>(c_args));
    }

    do {
        wait(&status);
        if(status == -1 && errno != ECHILD) {
            perror("Error during wait()");
            abort();
        }
    } while (status > 0);


    logger->info(filename, "Linking complete.");

    exit(0);
}