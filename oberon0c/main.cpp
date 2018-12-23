/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "lexer/Lexer.h"
#include "parser/Parser.h"

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    auto logger = std::make_unique<Logger>();
    logger->setLevel(LogLevel::DEBUG);
    auto lexer = std::make_unique<Lexer>(filename, logger.get());
    auto parser = std::make_unique<Parser>(lexer.get(), logger.get());
    parser->parse();
    logger->info(filename, "Parsing complete.");
    exit(0);
}