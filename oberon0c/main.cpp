/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "scanner/Scanner.h"

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    auto logger = std::make_unique<Logger>();
    logger->setLevel(LogLevel::DEBUG);
    auto scanner = std::make_unique<Scanner>(filename, logger.get());
    while (scanner->peekToken()->getType() != TokenType::eof) {
        auto token = scanner->nextToken();
        logger->info(filename, to_string(token->getType()));
    }
    logger->info(filename, "Scanning complete.");
    exit(0);
}