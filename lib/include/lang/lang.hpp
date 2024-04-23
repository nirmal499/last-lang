#pragma once

#include <types/types.hpp>
#include <lexer/lexer.hpp>
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
            std::unique_ptr<lang::Parser> m_parser{std::make_unique<lang::Parser>()};
            std::unique_ptr<lang::Interpreter> m_interpreter{std::make_unique<lang::Interpreter>()};
    };
}