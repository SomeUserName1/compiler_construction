/*
 * Main class of the Oberon-0 compiler.
 *
 * Created by Michael Grossniklaus on 12/14/17.
 */

#include <iostream>
#include <Lexer.h>
#include <Parser.h>
#include <SemanticAnalysis.h>

int main(const int argc, const char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: oberon0c <filename>" << std::endl;
        return 1;
    }
    std::string filename = argv[1];
    auto logger = std::make_shared<Logger>();
    logger->setLevel(LogLevel::DEBUG);

    // Lexical Analysis (Milestone 1)
    auto lexer = std::make_unique<Lexer>(filename, logger.get());
    logger->info(filename, "Lexical Analysis complete.");

    // Syntactic Analysis (Milestone 2)
    auto parser = std::make_unique<Parser>(lexer.get(), logger.get());
    auto parse_tree = parser->parse();
    std::cout << *parse_tree << std::endl;
    logger->info(filename, "Syntactic Analysis complete.");

    // Semantic Analysis (Milestone 3)
    auto checker = std::make_shared<SemanticAnalysis>(parse_tree, logger);
    auto ast = checker->check();

    logger->info(filename, "Semantic Analysis complete.");

    // Optimization 1, ..., n (Maybe Milestone 5)
    logger->info(filename, "Optimization steps not implemented yet.");

    // Code Generation (Milestone 4)
    logger->info(filename, "Code Generation not implemented yet.");


    exit(0);
}