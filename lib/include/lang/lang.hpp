#pragma once

#include <types/types.hpp>
#include <lexer/lexer.hpp>
#include <ast/ast_printer.hpp>
#include <parser/parser.hpp>
#include <interpreter/interpreter.hpp>

namespace lang
{
    class Lang
    {
        public:
            void run_source_code(const char* absolute_path_of_source_code);
            void run_repl();

        private:

            void run(std::string&& source);

        private:
            /** First lexer will be created then parser and then interpreter */
            std::unique_ptr<lang::Lexer> m_lexer{std::make_unique<lang::Lexer>()};

            std::unique_ptr<lang::Parser<std::string>> m_parser_ast_printer{std::make_unique<lang::Parser<std::string>>()};
            std::unique_ptr<lang::ast::ASTPrinter<std::string>> m_ast_printer{std::make_unique<lang::ast::ASTPrinter<std::string>>()};

            std::unique_ptr<lang::Parser<lang::util::object_t>> m_parser_ast_interpreter{std::make_unique<lang::Parser<lang::util::object_t>>()};
            std::unique_ptr<lang::Interpreter<lang::util::object_t>> m_ast_interpreter{std::make_unique<lang::Interpreter<lang::util::object_t>>()};
    };
}