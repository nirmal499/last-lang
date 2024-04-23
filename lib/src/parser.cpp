#include <parser/parser.hpp>

namespace lang
{
    std::pair<std::vector<lang::ast::Statement*>, std::vector<std::string>> Parser::parse(std::vector<lang::Token>&& tokens)
    {
        m_tokens = std::move(tokens);

        m_current = 0;
        /* It is important because we are moving from this class to outside at the end of tokenize function */
        m_errors = std::vector<std::string>();
        m_temp_exprs.clear();
        m_statements = std::vector<lang::ast::Statement*>();

        while(!this->is_at_end())
        {
            m_statements.emplace_back(this->parse_statement());
        }

        return std::make_pair(std::move(m_statements), std::move(m_errors));;
    }

    lang::ast::Statement* Parser::parse_statement()
    {
        if(this->match({lang::TokenType::PRINT}))
        {
            return this->parse_print_statement();
        }

        return this->parse_expression_statement();
    }

    lang::ast::Statement* Parser::parse_print_statement()
    {
        lang::ast::Expression* expr = this->parse_expression();

        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after value");
        auto print_statement = std::make_unique<lang::ast::PrintStatement>(expr);
        
        lang::ast::Statement* temp = print_statement.get();
        m_temp_stmts.emplace_back(std::move(print_statement));

        return temp;
    }

    lang::ast::Statement* Parser::parse_expression_statement()
    {
        lang::ast::Expression* expr = this->parse_expression();
        
        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after expression");
        auto expression_statement = std::make_unique<lang::ast::ExpressionStatement>(expr);
        
        lang::ast::Statement* temp = expression_statement.get();
        m_temp_stmts.emplace_back(std::move(expression_statement));

        return temp;
    }

    lang::ast::Expression* Parser::parse_expression()
    {
        return this->parse_equality();
    }

    lang::ast::Expression* Parser::parse_equality()
    {
        lang::ast::Expression* expr = this->parse_comparison();

        while(this->match({lang::TokenType::BANG_EQUAL, lang::TokenType::EQUAL_EQUAL}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_comparison();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }

    lang::ast::Expression* Parser::parse_comparison()
    {
        lang::ast::Expression* expr = this->parse_term();

        while(this->match({lang::TokenType::GREATER, lang::TokenType::GREATER_EQUAL, lang::TokenType::LESS, lang::TokenType::LESS_EQUAL}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_term();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }

    lang::ast::Expression* Parser::parse_term()
    {
        lang::ast::Expression* expr = this->parse_factor();

        while(this->match({lang::TokenType::MINUS, lang::TokenType::PLUS}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_factor();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }
    
    lang::ast::Expression* Parser::parse_factor()
    {
        lang::ast::Expression* expr = this->parse_unary();

        while(this->match({lang::TokenType::SLASH, lang::TokenType::STAR}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_unary();

            auto binary_expression = std::make_unique<lang::ast::BinaryExpression>(expr, op, right);
            expr = binary_expression.get();

            m_temp_exprs.emplace_back(std::move(binary_expression));
        }

        return expr;
    }

    lang::ast::Expression* Parser::parse_unary()
    {
        if(this->match({lang::TokenType::BANG, lang::TokenType::MINUS}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_unary();

            auto unary_expression = std::make_unique<lang::ast::UnaryExpression>(op, right);
            lang::ast::Expression* expr = unary_expression.get();

            m_temp_exprs.emplace_back(std::move(unary_expression));

            return expr;
        }

        return this->parse_primary();
    }

    lang::ast::Expression* Parser::parse_primary()
    {
        lang::ast::Expression* expr;

        if(this->match({lang::TokenType::FALSE}))
        {
            lang::util::object_t value = false;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::TRUE}))
        {
            lang::util::object_t value = true;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::NIL}))
        {
            lang::util::object_t value = lang::util::null;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::NUMBER, lang::TokenType::STRING}))
        {
            lang::util::object_t value = this->previous().m_literal;
            auto literal_expression = std::make_unique<lang::ast::LiteralExpression>(value);
            expr = literal_expression.get();

            m_temp_exprs.emplace_back(std::move(literal_expression));

            return expr;
        }

        if(this->match({lang::TokenType::LEFT_PAREN}))
        {
            expr = this->parse_expression();
            (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expecting ')' after expression.");

            auto grouping_expression = std::make_unique<lang::ast::GroupingExpression>(expr);
            expr = grouping_expression.get();

            m_temp_exprs.emplace_back(std::move(grouping_expression));

            return expr;
        }

        this->error(this->peek(), "Expect Expression.");

        return nullptr; // Unreachable
    }

    lang::Token Parser::consume(lang::TokenType type, std::string message)
    {
        if(this->check(type))
        {
            return this->advance();
        }

        this->error(this->peek(), message);

        return lang::Token{lang::TokenType::MYEOF, "DEAD END", lang::util::null, 1}; // Unreachable
    }
    
    void Parser::error(Token token, std::string message)
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
    
    void Parser::generate_error(int line, std::string message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());
    }
    
    bool Parser::match(const std::initializer_list<lang::TokenType>& matching_list)
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

    bool Parser::check(lang::TokenType type)
    {
        if(this->is_at_end())
        {
            return false;
        }

        return this->peek().m_type == type;
    }

    lang::Token Parser::advance()
    {
        if(!this->is_at_end())
        {
            m_current++;
        }

        return this->previous();
    }

    bool Parser::is_at_end()
    {
        return this->peek().m_type == lang::TokenType::MYEOF;
    }

    Token Parser::peek()
    {
        return m_tokens.at(m_current);
    }

    Token Parser::previous()
    {
        return m_tokens.at(m_current - 1);
    }
}