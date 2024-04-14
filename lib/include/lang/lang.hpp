#pragma once

#include <types/types.hpp>
#include <lexer/lexer.hpp>

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
            std::unique_ptr<lang::Lexer> m_lexer{std::make_unique<lang::Lexer>()};
    };
}