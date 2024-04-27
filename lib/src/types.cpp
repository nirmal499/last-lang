#include <types/types.hpp>
#include <interpreter/interpreter.hpp>
#include <environment/environment.hpp>

namespace lang
{
    namespace util
    {
        lang::util::object_t LLCallable::call(std::vector<lang::util::object_t>&& arguments)
        {
            if(flag_is_native_function)
            {
                /* We have a native function */
                return this->call_fn(std::move(arguments));
            }

            /*
                std::unique_ptr<lang::env::Environment> environment = std::make_unique<lang::env::Environment>(interpreter->get_environment());

                In this it was always the global_environment which is the parent here
            */
            std::unique_ptr<lang::env::Environment> environment = std::make_unique<lang::env::Environment>(closure);

            for(int i = 0; i < function_declaration_statement->params.size(); i++)
            {
                environment->define(function_declaration_statement->params.at(i).m_lexeme, arguments.at(i));
            }

            try{

                interpreter->execute_block(function_declaration_statement->body_stmts, environment.get());
                
            } catch(const lang::util::return_statement_throw& e)
            {
                return e.get_val();
            }

            return lang::util::null;
        }
    }
}