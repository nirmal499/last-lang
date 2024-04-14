#pragma once

#include <token/token_type.hpp>
#include <types/types.hpp>

namespace lang
{
    struct Token
    {
        lang::TokenType m_type;
        std::string m_lexeme; /* It stores the value represented by the Token. It is in std::string */
        lang::util::object_t m_literal; /* It corresponds to the type casted value of m_lexeme. e.g "5" -> 5, "wiz" -> "wiz", "var" -> lang::util::null */
        int m_line;

        Token(TokenType type, const std::string& lexeme, lang::util::object_t literal, int line)
            : m_type(type), m_lexeme(lexeme), m_literal(literal), m_line(line)
        {}
    };

    /* This anonymouse namespace solves the problem of multiple definitions of operator<< */
    namespace
    {
        std::ostream& operator<<(std::ostream& o,const lang::Token& token)
        {
            std::cout << lang::tokenType_map_to_string[token.m_type] << " '" << token.m_lexeme << "' ";
            std::visit(lang::util::PrintVisitor{}, token.m_literal);
            return o;
        }
    }
}