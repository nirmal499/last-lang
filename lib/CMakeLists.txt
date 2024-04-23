add_library(${LIBRARY_NAME} STATIC 
    src/lang.cpp
    src/lexer.cpp

    src/ast_printer.cpp

    src/parser.cpp

    src/interpreter.cpp
)

target_include_directories(${LIBRARY_NAME} 
    PUBLIC "include"
)