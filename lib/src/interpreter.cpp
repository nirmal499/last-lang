#include <interpreter/interpreter.hpp>

namespace lang
{

    lang::env::Environment* Interpreter::get_environment()
    {
        return m_environment;
    }

    /*****************************************native functions*******************************************/
    lang::util::object_t native_clock_function(std::vector<lang::util::object_t>&& arguments)
    {
        return 89.9;
    }
    /*****************************************native functions*******************************************/


    std::vector<std::string> Interpreter::interpret(std::vector<lang::ast::Statement*>&& statements)
    {
        m_errors.clear();

        auto environment = std::make_unique<lang::env::Environment>(nullptr);
        m_environment = environment.get();

        /*******************************************************************************************************************************************/
        lang::util::LLCallable* clock_callable = new lang::util::LLCallable(this, nullptr, true, 0, &native_clock_function, nullptr);

        m_environment->define("clock", clock_callable);

        m_temp_llcallables.push_back(clock_callable);

        /*******************************************************************************************************************************************/

        m_temp_envs.emplace_back(std::move(environment));

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
                                /* Throw evaluation_error */
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
        return this->evaluate(expression->expr);
    }

    lang::util::object_t Interpreter::visit(lang::ast::LiteralExpression* expression)
    {
        return expression->value;
    }

    lang::util::object_t Interpreter::visit(lang::ast::LogicalExpression* expression)
    {
        lang::util::object_t left_value = this->evaluate(expression->left);

        /****************************************************************************************************8*/
        lang::util::object_t temp_left_truthy_value = this->is_truthy(left_value);
        bool bool_result{false};
        if(auto* data = std::get_if<bool>(&temp_left_truthy_value))
        {
            bool_result = *data;
        }
        else
        {
            /* If our lang::util::object_t contains anything except lang::util::null then it is a TRUTHY */
            bool_result = true;
        }
        /****************************************************************************************************8*/

        if(expression->op.m_type == lang::TokenType::OR)
        {
            if(bool_result)
            {
                return left_value;
            }
        }
        else
        {
            if(!bool_result)
            {
                return left_value;
            }
        }

        return this->evaluate(expression->right);
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

    void Interpreter::visit(lang::ast::IfStatement* statement)
    {
        bool condition_result{false};

        lang::util::object_t temp_evaluated_condition_result = this->evaluate(statement->condition);
        lang::util::object_t temp_truthy_result = this->is_truthy(temp_evaluated_condition_result);

        /****************************************************************************************************8*/
        if(auto* data = std::get_if<bool>(&temp_truthy_result))
        {
            condition_result = *data;
        }
        else
        {
            /* If our lang::util::object_t contains anything except lang::util::null then it is a TRUTHY */
            condition_result = true;
        }
        /****************************************************************************************************8*/

        /* Here in if statement of c++. It needs a bool in condition block, but lang::util::object_t cannot be converted into a boolean expression */
        if(condition_result)
        {
            this->execute(statement->thenBranch);
        }
        else if (statement->elseBranch != nullptr)
        {
            this->execute(statement->elseBranch);
        }
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

    void Interpreter::visit(lang::ast::BlockStatement* statement)
    {
        auto environment = std::make_unique<lang::env::Environment>(m_environment);
        lang::env::Environment* temp = environment.get();

        m_temp_envs.emplace_back(std::move(environment));

        this->execute_block(statement->statements, temp);
    }

    void Interpreter::visit(lang::ast::FunctionStatement* statement)
    {
        lang::util::LLCallable* user_defined_function_callable = new lang::util::LLCallable(this, statement, false, statement->params.size(), nullptr, m_environment);
        
        m_environment->define(statement->name.m_lexeme, user_defined_function_callable);
        
        m_temp_llcallables.push_back(user_defined_function_callable);
    }

    void Interpreter::visit(lang::ast::WhileStatement* statement)
    {
        lang::util::object_t temp_evaluated_condition_result = this->evaluate(statement->condition);
        lang::util::object_t temp_truthy_value = this->is_truthy(temp_evaluated_condition_result);
        bool bool_result{false};
        if(auto* data = std::get_if<bool>(&temp_truthy_value))
        {
            bool_result = *data;
        }
        else
        {
            /* If our lang::util::object_t contains anything except lang::util::null then it is a TRUTHY */
            bool_result = true;
        }

        while(bool_result)
        {
            this->execute(statement->body);

            /*********************************************************************************************************************/
            temp_evaluated_condition_result = this->evaluate(statement->condition);
            temp_truthy_value = this->is_truthy(temp_evaluated_condition_result);
            if(auto* data = std::get_if<bool>(&temp_truthy_value))
            {
                bool_result = *data;
            }
            else
            {
                /* If our lang::util::object_t contains anything except lang::util::null then it is a TRUTHY */
                bool_result = true;
            }
            /*********************************************************************************************************************/

        }
    }

    void Interpreter::visit(lang::ast::ReturnStatement* statement)
    {
        lang::util::object_t evaluated_value = lang::util::null;
        if(statement->value != nullptr)
        {
            evaluated_value = this->evaluate(statement->value);
        }

        throw lang::util::return_statement_throw(evaluated_value);
    }

    void Interpreter::execute_block(const std::vector<lang::ast::Statement*>& stmts, lang::env::Environment* env)
    {
        lang::env::Environment* temp_env = m_environment;
        try
        {
            m_environment = env;
            for(auto const& stmt: stmts)
            {
                this->execute(stmt);
            }
        }
        catch(const lang::util::return_statement_throw& e)
        {
            /* finally */
            m_environment = temp_env; /* Restore back our environment */
            
            throw;
        }
        catch(...)
        {
            
        }

        /* finally */
        m_environment = temp_env; /* Restore back our environment */
    }

    lang::util::object_t Interpreter::visit(lang::ast::VariableExpression* expression)
    {
        lang::util::object_t value = lang::util::null;
        try
        {
            value = m_environment->get(expression->name);

        } catch(const std::exception& e)
        {
            this->generate_error(expression->name.m_line, e.what());
        }

        return value;
    }

    lang::util::object_t Interpreter::visit(lang::ast::AssignmentExpression* expression)
    {
        lang::util::object_t value = this->evaluate(expression->value);

        try
        {
            m_environment->assign(expression->name, value);

        } catch(const std::exception& e)
        {
            this->generate_error(expression->name.m_line, e.what());
        }

        return value;
    }

    lang::util::object_t Interpreter::visit(lang::ast::UnaryExpression* expression)
    {
        lang::util::object_t right = this->evaluate(expression->value);

        lang::util::object_t result;

        switch(expression->op.m_type)
        {
            case lang::TokenType::BANG:
                {
                    lang::util::object_t temp = this->is_truthy(right);
                    if(auto* data = std::get_if<bool>(&temp))
                    {
                        result = *data;
                    }
                    else
                    {
                        result = temp;
                    }
                    /* result = this->is_truthy(right); */
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

    lang::util::object_t Interpreter::visit(lang::ast::CallExpression* expression)
    {
        lang::util::object_t callee = this->evaluate(expression->callee);

        std::vector<lang::util::object_t> evaluated_arguments_value;
        for(auto const& argument: expression->arguments)
        {
            evaluated_arguments_value.emplace_back(this->evaluate(argument));
        }

        lang::util::LLCallable* function = nullptr;
        if(auto* data = std::get_if<lang::util::LLCallable*>(&callee))
        {
            function = *data;
        }
        else
        {
            this->generate_error(expression->closing_paren.m_line, "Can only call functions");
        }

        if(evaluated_arguments_value.size() != function->arity)
        {
            std::stringstream buffer;
            buffer << "Expected " << function->arity << " arguments but got " << evaluated_arguments_value.size() << ".";
            
            this->generate_error(expression->closing_paren.m_line, buffer.str());
        }
        return function->call(std::move(evaluated_arguments_value));
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
