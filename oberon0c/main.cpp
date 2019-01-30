/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"
#include "parser/CodeGen.h"

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

    // Code generation
    auto mainName = parse->getChildren().at(0)->getValue();
    auto mainCode = parse->getSymbolTable()->getSymbol(&mainName)->getAst();

    if (mainCode == nullptr) {
        logger->error(argv[1], "No main method found");
        return 1;
    }

    auto codeGen = std::make_shared<CodeGen>(parse->getSymbolTable(), mainCode);

    logger->info(filename, "Parsing complete.");

    exit(0);
}