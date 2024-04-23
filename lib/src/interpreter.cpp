#include <interpreter/interpreter.hpp>

namespace lang
{

    template<typename T>
    std::pair<lang::util::object_t, std::vector<std::string>> Interpreter<T>::interpret(lang::ast::Expression<T>* expression)
    {
        m_errors.clear();

        try
        {
            lang::util::object_t result = this->evaluate(expression);

            return std::make_pair(std::move(result), std::move(m_errors));
        }
        catch(const std::exception& e)
        {
            return std::make_pair(lang::util::null, std::move(m_errors));
        }
    }

    template<typename T>
    lang::util::object_t Interpreter<T>::evaluate(lang::ast::Expression<T>* expression)
    {
        return expression->accept(this);
    }

    template<typename T>
    T Interpreter<T>::visit(lang::ast::BinaryExpression<T>* expression)
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

    template<typename T>
    T Interpreter<T>::visit(lang::ast::GroupingExpression<T>* expression)
    {
        return this->evaluate(expression);
    }

    template<typename T>
    T Interpreter<T>::visit(lang::ast::LiteralExpression<T>* expression)
    {
        return expression->value;
    }

    template<typename T>
    T Interpreter<T>::visit(lang::ast::UnaryExpression<T>* expression)
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

    template<typename T>
    lang::util::object_t Interpreter<T>::is_truthy(const lang::util::object_t& object)
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

    template<typename T>
    bool Interpreter<T>::is_equal(const lang::util::object_t& object_A, const lang::util::object_t& object_B)
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

    template<typename T>
    void Interpreter<T>::generate_error(int line, std::string message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());

        throw lang::util::evaluation_error("Evaluation Error Caught");
    }

    /* https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor */
    template class Interpreter<lang::util::object_t>;
}