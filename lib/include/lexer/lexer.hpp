#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    class Lexer
    {
        public:
            std::pair<std::vector<lang::Token>, std::vector<std::string>> tokenize(std::string&& source);

        private:
            void scan_token();

            void read_identifier();

            /*
                A number literal is a series of digits optionally followed by a . and one or more trailing digits :- 1234, 12.34

                We don’t allow a leading or trailing decimal point, so these are both invalid:- .1234, 1234.
            */
            void read_number_literal();

            void read_string_literal();

            /* It consumes the next character in the source file and returns it */
            char advance();

            /* It is like conditional advance(). It only consumes the current character if it's what we're looking for. */
            bool match(char expected);

            /* 
                It's sort of like advance(), but doesn't consume the character. This is called lookahead
                At any moment the m_current points to the character which is under examination
                So, peek() gives us a peek at what m_current is pointing to
            */
            char peek();

            char peekNext();

            void add_token(TokenType type);

            void add_token(TokenType type, lang::util::object_t literal);

            /* It returns true if current reaches end of file */
            bool is_at_end();

            void generate_error(int line, std::string message);


            bool is_digit(char c);

            bool is_alpha(char c);

            bool is_aplha_numeric(char c);

        private:
            std::string m_source;
            std::size_t m_source_size;
            int m_start{0}; /* points to the first character in the lexeme being scanned */
            int m_current{0}; /* points to the character currently being considered */
            int m_line{1};

            std::vector<lang::Token> m_tokens;

            std::vector<std::string> m_errors;

            std::unordered_map<std::string, TokenType> m_keywords = {
                {"and", TokenType::AND},
                {"class", TokenType::CLASS},
                {"else", TokenType::ELSE},
                {"false", TokenType::FALSE},
                {"for", TokenType::FOR},
                {"fun", TokenType::FUN},
                {"if", TokenType::IF},
                {"nil", TokenType::NIL},
                {"or", TokenType::OR},
                {"print", TokenType::PRINT},
                {"return", TokenType::RETURN},
                {"super", TokenType::SUPER},
                {"this", TokenType::THIS},
                {"true", TokenType::TRUE},
                {"var", TokenType::VAR},
                {"while", TokenType::WHILE}
            };
    };
}