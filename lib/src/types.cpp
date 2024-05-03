#include <types/types.hpp>
#include <interpreter/interpreter.hpp>
#include <environment/environment.hpp>

namespace lang
{
    namespace util
    {

        LLCallable::LLCallable(
                
                    lang::Interpreter* interpreter, 
                    lang::ast::FunctionStatement* function_declaration_statement, 
                    bool flag_is_native_function,
                    size_t arity,
                    lang::util::object_t (*call_fn)(std::vector<lang::util::object_t>&& arguments),
                    lang::env::Environment* closure

            ) :     arity(arity), 
                    call_fn(call_fn), 
                    interpreter(interpreter), 
                    flag_is_native_function(flag_is_native_function), 
                    function_declaration_statement(function_declaration_statement),
                    closure(closure)
        {}

        LLCallable::~LLCallable()
        {
            if(new_environment != nullptr)
            {
                delete new_environment;
            }
        }

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
            // std::unique_ptr<lang::env::Environment> environment = std::make_unique<lang::env::Environment>(closure);

            new_environment = new lang::env::Environment(closure);

            for(int i = 0; i < function_declaration_statement->params.size(); i++)
            {
                new_environment->define(function_declaration_statement->params.at(i).m_lexeme, arguments.at(i));
            }

            try{

                interpreter->execute_block(function_declaration_statement->body_stmts, new_environment);
                
            } catch(const lang::util::return_statement_throw& e)
            {
                return e.get_val();
            }

            return lang::util::null;
        }
    }
}