#pragma once

#include <types/types.hpp>
#include <token/token.hpp>
// #include <ast/ast.hpp>
#include <interpreter/interpret.hpp>

namespace lang
{
    class Parser
    {
        public:
            Parser()
            {}

            std::pair<lang::interpret::Expression*, std::vector<std::string>> parse(std::vector<lang::Token>&& tokens);

        private:

            lang::interpret::Expression* parse_expression();

            lang::interpret::Expression* parse_equality();

            lang::interpret::Expression* parse_comparison();

            lang::interpret::Expression* parse_term();
            

            lang::interpret::Expression* parse_factor();

            lang::interpret::Expression* parse_unary();

            lang::interpret::Expression* parse_primary();

            lang::Token consume(lang::TokenType type, std::string message);

            void error(Token token, std::string message);

            void generate_error(int line, std::string message);
            
            bool match(const std::initializer_list<lang::TokenType>& matching_list);

            bool check(lang::TokenType type);

            lang::Token advance();

            bool is_at_end();

            Token peek();

            Token previous();

        private:
            std::vector<lang::Token> m_tokens;
            int m_current{0};

            std::vector<std::unique_ptr<lang::interpret::Expression>> m_temp_exprs;

            std::vector<std::string> m_errors;
    };
}