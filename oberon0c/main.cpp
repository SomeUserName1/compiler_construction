/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include "scanner/Scanner.h"
#include "parser/Parser.h"



//Testsection
#include "parser/st/SymbolTable.h"
int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    auto logger = std::make_unique<Logger>();
    logger->setLevel(LogLevel::DEBUG);
    auto scanner = std::make_unique<Scanner>(filename, logger.get());
    auto parser = std::make_unique<Parser>(scanner.get(), logger.get());
    parser->parse();
    logger->info(filename, "Parsing complete.");





	// Testsection
	auto test = std::make_unique<SymbolTable>();
	std::string identifier = std::string("lala");
	Symbol * symbol = test->getSymbol(&identifier);
	std::string * name = symbol->getName();
	logger->info(filename, *name);

	std::cin.ignore();
    exit(0);
}