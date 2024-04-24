#include <interpreter/interpreter.hpp>

namespace lang
{
    std::vector<std::string> Interpreter::interpret(std::vector<lang::ast::Statement*>&& statements)
    {
        m_errors.clear();

        try
        {
            for(const auto& stmt: statements)
            {
                this->execute(stmt);
            }

            return std::move(m_errors);
        }
        catch(const std::exception& e)
        {
            return std::move(m_errors);
        }
    }

    lang::util::object_t Interpreter::evaluate(lang::ast::Expression* expression)
    {
        return expression->accept(this);
    }

    void Interpreter::execute(lang::ast::Statement* statement)
    {
        statement->accept(this);
    }

    lang::util::object_t Interpreter::visit(lang::ast::BinaryExpression* expression)
    {
        lang::util::object_t left = this->evaluate(expression->left);
        lang::util::object_t right = this->evaluate(expression->right);

        switch(expression->op.m_type)
        {
            case lang::TokenType::GREATER:
            case lang::TokenType::GREATER_EQUAL:
            case lang::TokenType::LESS:
            case lang::TokenType::LESS_EQUAL:
            case lang::TokenType::MINUS:
            case lang::TokenType::SLASH:
            case lang::TokenType::STAR:
                {
                    lang::util::object_t result;
                    
                    auto* left_data = std::get_if<double>(&left);
                    auto* right_data = std::get_if<double>(&right);

                    if(left_data && right_data)
                    {
                        switch(expression->op.m_type)
                        {
                            case lang::TokenType::GREATER:
                                result = (*left_data) > (*right_data);
                                break;

                            case lang::TokenType::GREATER_EQUAL:
                                result = (*left_data) >= (*right_data);
                                break;

                            case lang::TokenType::LESS:
                                result = (*left_data) < (*right_data);
                                break;

                            case lang::TokenType::LESS_EQUAL:
                                result = (*left_data) <= (*right_data);
                                break;

                            case lang::TokenType::MINUS:
                                result = (*left_data) - (*right_data);
                                break;

                            case lang::TokenType::SLASH:
                                result = (*left_data) / (*right_data);
                                break;
                            
                            case lang::TokenType::STAR:
                                result = (*left_data) * (*right_data);
                                break;

                            default:
                                this->generate_error(expression->op.m_line, "Something else is happened instead of MINUS;SLASH;STAR");
                                result = lang::util::null;
                                break;
                        }
                    }
                    else
                    {
                        this->generate_error(expression->op.m_line, "MINUS is allowed in <double> only");
                        result = lang::util::null;
                    }

                    return result;

                }
            case lang::TokenType::PLUS:
                {
                    lang::util::object_t result;

                    auto* left_data_double = std::get_if<double>(&left);
                    auto* right_data_double = std::get_if<double>(&right);

                    auto* left_data_string = std::get_if<std::string>(&left);
                    auto* right_data_string = std::get_if<std::string>(&right);

                    if(left_data_double && right_data_double)
                    {
                        result = (*left_data_double) + (*right_data_double);
                    }
                    else if(left_data_string && right_data_string)
                    {
                        result = (*left_data_string) + (*right_data_string);
                    }
                    else
                    {
                        this->generate_error(expression->op.m_line, "PLUS is allowed in <double, string> only");
                        result = lang::util::null;
                    }

                    return result;
                }

            case lang::TokenType::BANG_EQUAL:
                {
                    return !this->is_equal(left, right);
                }
            case lang::TokenType::EQUAL_EQUAL:
                {
                    return this->is_equal(left, right);
                }
            
        }


        return lang::util::null;
    }

    lang::util::object_t Interpreter::visit(lang::ast::GroupingExpression* expression)
    {
        return this->evaluate(expression);
    }

    lang::util::object_t Interpreter::visit(lang::ast::LiteralExpression* expression)
    {
        return expression->value;
    }

    void Interpreter::visit(lang::ast::ExpressionStatement* statement)
    {
        (void)this->evaluate(statement->expr);
        
        return;
    }

    void Interpreter::visit(lang::ast::PrintStatement* statement)
    {
        lang::util::object_t value = this->evaluate(statement->expr);
        std::visit(lang::util::PrintVisitor{}, value); /* This is our language's print statement */

        return;
    }
    
    void Interpreter::visit(lang::ast::VarStatement* statement)
    {
        lang::util::object_t value = lang::util::null;
        if(statement->initializer != nullptr)
        {
            value = this->evaluate(statement->initializer);
        }
        m_environment->define(statement->name.m_lexeme, value);

        return;
    }

    lang::util::object_t Interpreter::visit(lang::ast::VariableExpression* expression)
    {
        return m_environment->get(expression->name);
    }

    lang::util::object_t Interpreter::visit(lang::ast::UnaryExpression* expression)
    {
        lang::util::object_t right = this->evaluate(expression->value);

        lang::util::object_t result;

        switch(expression->op.m_type)
        {
            case lang::TokenType::BANG:
                {
                    result = this->is_truthy(right);
                    break;
                }
            case lang::TokenType::MINUS:
                {
                    if(auto* data = std::get_if<double>(&right))
                    {
                        result = (*data) * -1;
                    }
                    else
                    {
                        this->generate_error(expression->op.m_line, "MINUS is allowed in <double> only");
                        result = lang::util::null;
                    }

                    break;
                }
        }

        return result;
    }

    lang::util::object_t Interpreter::is_truthy(const lang::util::object_t& object)
    {
        if(auto *data = std::get_if<lang::util::null_t>(&object))
        {
            return false;
        }

        if(auto *data = std::get_if<bool>(&object))
        {
            return object;
        }

        return true; /* For everything else just return true */
    }

    bool Interpreter::is_equal(const lang::util::object_t& object_A, const lang::util::object_t& object_B)
    {
        auto *data_object_A = std::get_if<lang::util::null_t>(&object_A);
        auto *data_object_B = std::get_if<lang::util::null_t>(&object_B);

        if(data_object_A && data_object_B)
        {
            return true;
        }
        
        if(auto *data = std::get_if<lang::util::null_t>(&object_A))
        {
            /* object_A is null but object_B is not null */
            return false;
        }


        auto *A_data_bool_object = std::get_if<bool>(&object_A);
        auto *B_data_bool_object = std::get_if<bool>(&object_B);

        auto *A_data_string_object = std::get_if<std::string>(&object_A);
        auto *B_data_string_object = std::get_if<std::string>(&object_B);

        auto *A_data_object_double = std::get_if<double>(&object_A);
        auto *B_data_object_double = std::get_if<double>(&object_B);

        if(A_data_bool_object && B_data_bool_object)
        {
            return (*A_data_bool_object) == (*B_data_bool_object);
        }
        else if(A_data_string_object &&  B_data_string_object)
        {
            return (*A_data_string_object) == (*B_data_string_object);
        }
        else if(A_data_object_double && B_data_object_double)
        {
            return (*A_data_object_double) == (*B_data_object_double);
        }

        return false;
    }

    void Interpreter::generate_error(int line, std::string message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());

        throw lang::util::evaluation_error("Evaluation Error Caught");
    }
}