#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <sstream>
#include <exception>
#include <iomanip>
#include <algorithm>
#include <functional>

namespace lang
{
    namespace ast
    {
        struct FunctionStatement;
    }
    class Interpreter;
    
    namespace env
    {
        class Environment;
    }

    namespace util
    {
        enum class MYTYPE
        {
            NIL
        };

        struct LLCallable;

        /* This anonymouse namespace solves the problem of multiple definitions of null_t */
        namespace
        {
            using null_t = lang::util::MYTYPE;
            null_t null = MYTYPE::NIL;

            using object_t = std::variant<double, null_t, std::string, bool, lang::util::LLCallable*>;
        }

        struct LLCallable
        {
            size_t arity{0};
            lang::util::object_t (*call_fn)(std::vector<lang::util::object_t>&& arguments) = nullptr;
            bool flag_is_native_function{false};
            lang::Interpreter* interpreter = nullptr;
            lang::ast::FunctionStatement* function_declaration_statement = nullptr;
            lang::env::Environment* closure = nullptr;

            lang::env::Environment* new_environment = nullptr;

            LLCallable(
                
                    lang::Interpreter* interpreter, 
                    lang::ast::FunctionStatement* function_declaration_statement, 
                    bool flag_is_native_function,
                    size_t arity,
                    lang::util::object_t (*call_fn)(std::vector<lang::util::object_t>&& arguments),
                    lang::env::Environment* closure

            );

            ~LLCallable();

            lang::util::object_t call(std::vector<lang::util::object_t>&& arguments);
        };

        struct PrintVisitor
        {
            void operator()(double value) const { std::cout << value << "\n"; }
            void operator()(const std::string& value) const { std::cout << value << "\n"; }
            void operator()(bool value) const { std::cout << std::boolalpha << value << "\n"; }
            void operator()(lang::util::LLCallable* llcallable)
            {
                std::cout << "<NATIVE FN>\n";
            }
            void operator()(null_t value) const
            {
                std::cout << "MYTYPE::NIL\n";
            }
        };

        /* Custom Exception */
        class parser_error: public std::exception
        {
            public:
                /* Constructor with a message */
                parser_error(const char* message): msg(message){}

                /* Override what() method to provide error message */
                virtual const char* what() const throw() {
                    return msg.c_str();
                }
            
            private:
                std::string msg;
        };

        /* Custom Exception */
        class evaluation_error: public std::exception
        {
            public:
                /* Constructor with a message */
                evaluation_error(const char* message): msg(message){}

                /* Override what() method to provide error message */
                virtual const char* what() const throw() {
                    return msg.c_str();
                }
            
            private:
                std::string msg;
        };

        class return_statement_throw: public std::exception
        {
            public:
                return_statement_throw(const lang::util::object_t& value)
                    : value(value)
                {}

                ~return_statement_throw()
                {}

                /* Override what() method to provide error message */
                virtual const char* what() const throw() {
                    return "IT IS JUST FOR RETURN STATEMENT";
                }

                lang::util::object_t get_val() const
                {
                    return value;
                }

            private:
                lang::util::object_t value;
        };

    }
}