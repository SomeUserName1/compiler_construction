cmake_minimum_required(VERSION 3.0)
project(oberon0c CXX)

# Compiler, flags, optimizer/sanitizer settings
SET(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)

# SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++17 -fsanitize=address -fno-omit-frame-pointer -fno-optimize-sibling-calls")
#  -fsanitize=memory
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++17 -O3")

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
include_directories(codegen)

# Sub-modules
set(util util/Logger.cpp)
ADD_LIBRARY(logger ${util})

set(scanner
		scanner/Token.cpp
		scanner/NumberToken.cpp
		scanner/StringToken.cpp
		scanner/IdentToken.cpp
		scanner/Scanner.cpp
		)
ADD_LIBRARY(scan ${scanner})


set(parser
		parser/Parser.cpp
		parser/ast/Node.cpp
		parser/st/SymbolTable.cpp
		parser/st/Symbol.cpp
		parser/ast/BuildAST.cpp
		parser/ast/ASTNode.cpp
		)
ADD_LIBRARY(parse ${parser})

set(code_generator
		codegen/CodeGen.cpp
		)
ADD_LIBRARY(codegen ${code_generator})

add_executable(oberon0c main.cpp)

TARGET_LINK_LIBRARIES(oberon0c logger scan parse codegen)
