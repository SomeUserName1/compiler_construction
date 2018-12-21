cmake_minimum_required(VERSION 3.0)
project(oberon0c)

# Compiler, flags, optimizer/sanitizer settings
SET(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++17 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
#  -fsanitize=memory
SET(ASAN_OPTIONS symbolize=1)
SET(ASAN_OPTIONS detect_leaks=1)
SET(ASAN_SYMBOLIZER_PATH $(which llvm-symbolizer-6.0))

SET(MSAN_OPTIONS detect_leaks=1)
SET(MSAN_OPTIONS symbolize=1)
SET(MSAN_SYMBOLIZER_PATH $(which llvm-symbolizer-6.0))

# Include paths
include_directories(util)
include_directories(scanner)
include_directories(parser)
include_directories(ir)

# Sub-modules
# A simple logger
set(logger util/Logger.cpp)
ADD_LIBRARY(logger_lib ${logger})

# The Scanner/Tokenizer/Lexer: Input is a file in Oberon-0, outputs a stream of Tokens
set(scanner
		scanner/Token.cpp
		scanner/NumberToken.cpp
		scanner/StringToken.cpp
		scanner/IdentToken.cpp
		scanner/Scanner.cpp)
ADD_LIBRARY(scanner_lib ${scanner})

# The Parser: Input is a stream of token, throws on syntax errors, outputs a parse tree
set(parser
		parser/Parser.cpp
        parser/parse_tree/Node.cpp)
ADD_LIBRARY(parser_lib ${parser})

# The AST Nodes: Basic control flow and data structures that are type checked
set(ast_nodes
        ir/ast_nodes/AssignmentNode.cpp
        ir/ast_nodes/ControlFlowNode.cpp
        ir/ast_nodes/BuiltInOpNode.cpp
        ir/ast_nodes/CallNode.cpp)

# The symbol table nodes: Declarations, scoping i.e. the data types that are checked for collisions
set(sym_table_nodes
        ir/sym_table_nodes/DeclarationNode.cpp
        ir/sym_table_nodes/NumberNode.cpp
        ir/sym_table_nodes/ArrayNode.cpp
        ir/sym_table_nodes/RecordNode.cpp
        ir/sym_table_nodes/HigherOrderTypeNode.cpp
        ir/sym_table_nodes/ProcedureNode.cpp )

# The IR module: Converting from the parse tree to an ast, checking types and collisions. If the code is in the form of
# the ast, syntactic and semantic correctness shall be guaranteed as invariance (no collisions, correct types, ...)
# As specified in the Milestone description this is our intermediate representation, but may be easily extended to
# generate fancier IRs ( see here: https://llvm.org/docs/tutorial/LangImpl03.html)
set(ir
        ir/ASTNode.cpp
        ir/SymbolTable.cpp
        ir/TypeChecker.cpp
        ${ast_nodes}
        ${sym_table_nodes})
ADD_LIBRARY(ir_lib ${ir})

# Main executable
add_executable(oberon0c main.cpp)
TARGET_LINK_LIBRARIES(oberon0c logger_lib scanner_lib parser_lib ir_lib)
