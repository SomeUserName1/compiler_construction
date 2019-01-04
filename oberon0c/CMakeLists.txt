cmake_minimum_required(VERSION 3.0)
project(oberon0c)

# Compiler, flags, optimizer/sanitizer settings
SET(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++17 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
#  -fsanitize=memory spits out uninitialized memory on an ifstream... even though its initialized ... wtf
#
SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer-6.0))

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer-6.0))

# Include paths
include_directories(util)
include_directories(lexer)
include_directories(parser)

# Sub-modules
# A simple logger
set(util util/Logger.cpp)
ADD_LIBRARY(util ${util})

# BEGIN liblex
# The Scanner/Tokenizer/Lexer: Input is a file in Oberon-0, outputs a stream of Tokens
set(lexer
		lexer/Token.cpp
		lexer/NumberToken.cpp
		lexer/StringToken.cpp
		lexer/IdentToken.cpp
		lexer/Lexer.cpp)
ADD_LIBRARY(lex ${lexer})
# END liblex

# The AST Nodes: Basic control flow and data structures that are type checked
set(ast
		parser/ast/AssignmentNode.cpp
		parser/ast/ControlFlowNode.cpp
		parser/ast/CallNode.cpp
		parser/ast/ArithmeticOpNode.cpp
		parser/ast/BooleanOpNode.cpp
		parser/ast/ComparatorNode.cpp)

# The symbol table nodes: Declarations, scoping i.e. the data types that are checked for collisions
set(symbol_table
		parser/symbol_table/SymbolScopeNode.cpp
        parser/symbol_table/DeclarationNode.cpp
		parser/symbol_table/NumberNode.cpp
		parser/symbol_table/ArrayNode.cpp
		parser/symbol_table/RecordNode.cpp
		parser/symbol_table/TypeNode.cpp
		parser/symbol_table/ProcedureNode.cpp
		parser/symbol_table/BooleanNode.cpp)


# The Parser: Input is a stream of token, throws on syntax errors, outputs a parse tree.
# TypeChecker shall convert the parse tree to an ast, checking types and symbol collisions.
# If the code is in the form of the ast, syntactic and semantic correctness shall be guaranteed as invariance
set(parser
		parser/Node.cpp
		parser/parse_tree/ParserNode.cpp
		${ast}
		${symbol_table}
		parser/Parser.cpp
        parser/SemanticAnalysis.cpp)
ADD_LIBRARY(parse ${parser})

# Main executable
add_executable(oberon0c main.cpp)
TARGET_LINK_LIBRARIES(oberon0c util lex parse)
