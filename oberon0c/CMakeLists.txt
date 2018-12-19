cmake_minimum_required(VERSION 3.0)
project(oberon0c)


#SET(CMAKE_CXX_FLAGS " -c -Wall  -std=c++17")
SET(CMAKE_CXX_COMPILER clang++)
SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic -std=c++17")
#-fsanitize=address -fsanitize=memory

set(CMAKE_CXX_STANDARD 17)

include_directories(scanner)
include_directories(parser)
include_directories(util)

set(logger util/Logger.cpp)
set(scanner
		scanner/Token.cpp
		scanner/NumberToken.cpp
		scanner/StringToken.cpp
		scanner/IdentToken.cpp
		scanner/Scanner.cpp
		)
set(parser
		parser/Parser.cpp
        parser/parse_tree/Node.cpp
		)
set(ir
        ir/ast.cpp
        ir/symbol_table.cpp
        # op node var node const node
        # type checker decl checker usage checker statement checker
        )

add_executable(oberon0c
		${logger}
		${scanner}
		${parser}
        main.cpp
        ir/ast.cpp ir/ast.h)
