cmake_minimum_required(VERSION 3.12)
project(oberon0c)


SET(CMAKE_CXX_FLAGS " -c -W4 /EHsc")
#SET(CMAKE_CXX_COMPILER clang++)
#SET(CMAKE_CXX_FLAGS "-Wall -Wpedantic") #-fsanitize=address -fsanitize=memory

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
		parser/ast/Node.cpp
		parser/st/SymbolTable.cpp
		parser/st/Symbol.cpp
		)

add_executable(oberon0c
		${logger}
		${scanner}
		${parser}
        main.cpp
		)