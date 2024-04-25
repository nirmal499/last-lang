#pragma once

#include <types/types.hpp>
#include <token/token.hpp>
#include <ast/ast.hpp>

namespace lang
{
    class Parser
    {
        public:
            Parser()
            {}

            std::pair<std::vector<lang::ast::Statement*>, std::vector<std::string>> parse(std::vector<lang::Token>&& tokens);

        private:
            lang::ast::Statement* parse_declaration();

            lang::ast::Statement* parse_var_declaration();

            lang::ast::Statement* parse_statement();
            lang::ast::Statement* parse_print_statement();
            lang::ast::Statement* parse_expression_statement();
            std::vector<lang::ast::Statement*> parse_block();
            lang::ast::Statement* parse_while_statement();
            lang::ast::Statement* parse_function_statement();

            lang::ast::Statement* parse_if_statement();
            
            lang::ast::Expression* parse_expression();
            lang::ast::Expression* parse_call_expression();
            lang::ast::Expression* parse_assignment();

            lang::ast::Expression* parse_equality();
            lang::ast::Expression* parse_logical_or_expression();
            lang::ast::Expression* parse_logical_and_expression();

            lang::ast::Expression* parse_comparison();

            lang::ast::Expression* parse_term();
            

            lang::ast::Expression* parse_factor();

            lang::ast::Expression* parse_unary();

            lang::ast::Expression* parse_primary();

            lang::Token consume(lang::TokenType type, std::string message);

            void error(const Token& token, std::string message);

            void generate_error(int line, std::string message);
            
            bool match(const std::initializer_list<lang::TokenType>& matching_list);

            bool check(lang::TokenType type);

            lang::Token advance();

            bool is_at_end();

            Token peek();

            Token previous();

            void synchronize_after_an_exception();

            lang::ast::Expression* finish_call(lang::ast::Expression* callee);

        private:
            std::vector<lang::Token> m_tokens;
            int m_current{0};

            std::vector<std::unique_ptr<lang::ast::Expression>> m_temp_exprs;
            std::vector<std::unique_ptr<lang::ast::Statement>> m_temp_stmts;

            std::vector<std::string> m_errors;

            std::vector<lang::ast::Statement*> m_statements;

    };
}