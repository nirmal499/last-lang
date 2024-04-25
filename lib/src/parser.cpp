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
            m_statements.emplace_back(this->parse_declaration());
        }

        return std::make_pair(std::move(m_statements), std::move(m_errors));;
    }

    lang::ast::Statement* Parser::parse_declaration()
    {
        try
        {
            if(this->match({lang::TokenType::VAR}))
            {
                return this->parse_var_declaration();
            }

            return this->parse_statement();
        }
        catch(const std::exception& e)
        {
            this->synchronize_after_an_exception();
            return nullptr;
        }
    }

    lang::ast::Statement* Parser::parse_var_declaration()
    {
        lang::Token name = this->consume(lang::TokenType::IDENTIFIER, "Expect variable name.");
        lang::ast::Expression* initializer = nullptr;

        if(this->match({lang::TokenType::EQUAL}))
        {
            initializer = this->parse_expression();
        }

        (void)this->consume(lang::TokenType::SEMICOLON, "Expect ';' after variable declaration");

        auto var_statement = std::make_unique<lang::ast::VarStatement>(name, initializer);
        lang::ast::Statement* temp = var_statement.get();

        m_temp_stmts.emplace_back(std::move(var_statement));

        return temp;
    }

    lang::ast::Statement* Parser::parse_statement()
    {
        if(this->match({lang::TokenType::IF}))
        {
            return this->parse_if_statement();
        }

        if(this->match({lang::TokenType::PRINT}))
        {
            return this->parse_print_statement();
        }

        if(this->match({lang::TokenType::WHILE}))
        {
            return this->parse_while_statement();
        }

        if(this->match({lang::TokenType::LEFT_BRACE}))
        {
            std::vector<lang::ast::Statement*> stmts = this->parse_block();
            auto block_statement = std::make_unique<lang::ast::BlockStatement>(std::move(stmts));
            lang::ast::Statement* temp = block_statement.get();

            m_temp_stmts.emplace_back(std::move(block_statement));
            return temp;
        }

        return this->parse_expression_statement();
    }

    lang::ast::Statement* Parser::parse_while_statement()
    {
        (void)this->consume(lang::TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
        lang::ast::Expression* condition = this->parse_expression();
        (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expect ')' after condition.");
        lang::ast::Statement* body = this->parse_statement();

        auto while_statement = std::make_unique<lang::ast::WhileStatement>(condition, body);
        lang::ast::Statement* temp = while_statement.get();

        m_temp_stmts.emplace_back(std::move(while_statement));

        return temp;
    }

    lang::ast::Statement* Parser::parse_if_statement()
    {
        (void)this->consume(lang::TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
        lang::ast::Expression* condition = this->parse_expression();
        (void)this->consume(lang::TokenType::RIGHT_PAREN, "Expect ')' after 'if'.");

        lang::ast::Statement* thenBranch = this->parse_statement();
        lang::ast::Statement* elseBranch = nullptr;

        if(this->match({lang::TokenType::ELSE}))
        {
            elseBranch = this->parse_statement();
        }

        auto if_statement = std::make_unique<lang::ast::IfStatement>(condition, thenBranch, elseBranch);
        lang::ast::Statement* temp = if_statement.get();

        m_temp_stmts.emplace_back(std::move(if_statement));

        return temp;
    }

    std::vector<lang::ast::Statement*> Parser::parse_block()
    {
        std::vector<lang::ast::Statement*> statements;

        while(!this->check(lang::TokenType::RIGHT_BRACE) && !this->is_at_end())
        {
            statements.emplace_back(this->parse_declaration());
        }

        (void)this->consume(lang::TokenType::RIGHT_BRACE, "Expect '}' after block");

        return statements;
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
        return this->parse_assignment();
    }

    lang::ast::Expression* Parser::parse_assignment()
    {
        lang::ast::Expression* expr = this->parse_logical_or_expression();

        if(this->match({lang::TokenType::EQUAL}))
        {
            lang::Token equals = this->previous();
            lang::ast::Expression* value = this->parse_assignment();

            if(lang::ast::VariableExpression* var_expr = dynamic_cast<lang::ast::VariableExpression*>(expr))
            {
                lang::Token name = var_expr->name;
                auto assignment_expression = std::make_unique<lang::ast::AssignmentExpression>(name, value);
                lang::ast::Expression* temp = assignment_expression.get();

                m_temp_exprs.emplace_back(std::move(assignment_expression));

                return temp;
            }

            this->error(equals, "Invalid assignment target");
        }

        return expr;
    }

    lang::ast::Expression* Parser::parse_logical_or_expression()
    {
        lang::ast::Expression* expr = this->parse_logical_and_expression();
        
        while(this->match({lang::TokenType::OR}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_logical_and_expression();
            
            auto logical_expression = std::make_unique<lang::ast::LogicalExpression>(expr, op, right);
            lang::ast::Expression* temp = logical_expression.get();
            expr = temp;

            m_temp_exprs.emplace_back(std::move(logical_expression));
        }

        return expr;
    }

    lang::ast::Expression* Parser::parse_logical_and_expression()
    {
        lang::ast::Expression* expr = this->parse_equality();
        
        while(this->match({lang::TokenType::AND}))
        {
            lang::Token op = this->previous();
            lang::ast::Expression* right = this->parse_equality();
            
            auto logical_expression = std::make_unique<lang::ast::LogicalExpression>(expr, op, right);
            lang::ast::Expression* temp = logical_expression.get();
            expr = temp;

            m_temp_exprs.emplace_back(std::move(logical_expression));
        }

        return expr;
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

        if(this->match({lang::TokenType::IDENTIFIER}))
        {
            auto variable_expression = std::make_unique<lang::ast::VariableExpression>(this->previous());
            expr = variable_expression.get();

            m_temp_exprs.emplace_back(std::move(variable_expression));

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

    void Parser::synchronize_after_an_exception()
    {
        this->advance();

        while(!this->is_at_end())
        {
            if(this->previous().m_type == lang::TokenType::SEMICOLON)
            {
                /* 
                    We have reached the end of the statement which caused an exception. 
                    So we return inorder to parse the other statements after this error statements
                */
                return;
            }

            switch(this->peek().m_type)
            {
                case lang::TokenType::CLASS:
                case lang::TokenType::FUN:
                case lang::TokenType::VAR:
                case lang::TokenType::FOR:
                case lang::TokenType::IF:
                case lang::TokenType::WHILE:
                case lang::TokenType::PRINT:
                case lang::TokenType::RETURN:
                    return;

            }

            this->advance();
        }
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