#include <parser/parser.hpp>

namespace lang
{
    template<typename T>
    std::pair<lang::ast::Expression<T>*, std::vector<std::string>> Parser<T>::parse(std::vector<lang::Token>&& tokens)
    {
        m_tokens = std::move(tokens);

        m_current = 0;
        /* It is important because we are moving from this class to outside at the end of tokenize function */
        m_errors = std::vector<std::string>();
        m_temp_exprs.clear();

        try
        {
            lang::ast::Expression<T>* expr = this->parse_expression();

            return std::make_pair(expr, std::move(m_errors));
        }
        catch(const std::exception& e)
        {
            return std::make_pair(nullptr, std::move(m_errors));
        }
    }

    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_expression()
    {
        return this->parse_equality();
    }

    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_equality()
    {
        lang::ast::Expression<T>* expr = this->parse_comparison();

        while(this->match({lang::TokenType::BANG_EQUAL, lang::TokenType::EQUAL_EQUAL}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression<T>* right = this->parse_comparison();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression<T>>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }

    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_comparison()
    {
        lang::ast::Expression<T>* expr = this->parse_term();

        while(this->match({lang::TokenType::GREATER, lang::TokenType::GREATER_EQUAL, lang::TokenType::LESS, lang::TokenType::LESS_EQUAL}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression<T>* right = this->parse_term();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression<T>>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }

    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_term()
    {
        lang::ast::Expression<T>* expr = this->parse_factor();

        while(this->match({lang::TokenType::MINUS, lang::TokenType::PLUS}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression<T>* right = this->parse_factor();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression<T>>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }
    
    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_factor()
    {
        lang::ast::Expression<T>* expr = this->parse_unary();

        while(this->match({lang::TokenType::SLASH, lang::TokenType::STAR}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression<T>* right = this->parse_unary();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression<T>>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }

    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_unary()
    {
        if(this->match({lang::TokenType::BANG, lang::TokenType::MINUS}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression<T>* right = this->parse_unary();

            auto unary_expression = std::make_unique<lang::ast::UnaryExpression<T>>(op, right);
            lang::ast::Expression<T>* expr = unary_expression.get();

            m_temp_exprs.emplace_back(std::move(unary_expression));

            return expr;
        }

        return this->parse_primary();
    }

    template<typename T>
    lang::ast::Expression<T>* Parser<T>::parse_primary()
    {
        lang::ast::Expression<T>* expr;

        if(this->match({lang::TokenType::FALSE}))
        {
            lang::util::object_t value = false;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression<T>>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::TRUE}))
        {
            lang::util::object_t value = true;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression<T>>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::NIL}))
        {
            lang::util::object_t value = lang::util::null;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression<T>>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::NUMBER, lang::TokenType::STRING}))
        {
            lang::util::object_t value = this->previous().m_literal;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression<T>>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::LEFT_PAREN}))
        {
            expr = this->parse_expression();
            (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expecting ')' after expression.");

            auto grouping_expression = std::make_unique<lang::ast::GroupingExpression<T>>(expr);
            expr = grouping_expression.get();

            m_temp_exprs.emplace_back(std::move(grouping_expression));

            return expr;
        }

        this->error(this->peek(), "Expect Expression.");

        return nullptr; // Unreachable
    }

    template<typename T>
    lang::Token Parser<T>::consume(lang::TokenType type, std::string message)
    {
        if(this->check(type))
        {
            return this->advance();
        }

        this->error(this->peek(), message);

        return lang::Token{lang::TokenType::MYEOF, "DEAD END", lang::util::null, 1}; // Unreachable
    }
    
    template<typename T>
    void Parser<T>::error(Token token, std::string message)
    {
        if(token.m_type == lang::TokenType::MYEOF)
        {
            this->generate_error(token.m_line, " at end: " + message);
        }
        else
        {
            this->generate_error(token.m_line, " at '" + token.m_lexeme + "' " + message);
        }

        throw lang::util::parser_error("Parser Error Caught");
    }
    
    template<typename T>
    void Parser<T>::generate_error(int line, std::string message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());
    }
    
    template<typename T>
    bool Parser<T>::match(const std::initializer_list<lang::TokenType>& matching_list)
    {
        for(const auto& type: matching_list)
        {
            if(this->check(type))
            {
                this->advance();
                return true;
            }
        }

        return false;
    }

    template<typename T>
    bool Parser<T>::check(lang::TokenType type)
    {
        if(this->is_at_end())
        {
            return false;
        }

        return this->peek().m_type == type;
    }

    template<typename T>
    lang::Token Parser<T>::advance()
    {
        if(!this->is_at_end())
        {
            m_current++;
        }

        return this->previous();
    }

    template<typename T>
    bool Parser<T>::is_at_end()
    {
        return this->peek().m_type == lang::TokenType::MYEOF;
    }

    template<typename T>
    Token Parser<T>::peek()
    {
        return m_tokens.at(m_current);
    }

    template<typename T>
    Token Parser<T>::previous()
    {
        return m_tokens.at(m_current - 1);
    }

    /* https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor */
    template class Parser<std::string>;
    template class Parser<lang::util::object_t>;
}