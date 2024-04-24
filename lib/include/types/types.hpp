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

namespace lang
{
    namespace util
    {
        enum class MYTYPE
        {
            NIL
        };

        /* This anonymouse namespace solves the problem of multiple definitions of null_t */
        namespace
        {
            using null_t = lang::util::MYTYPE;
            null_t null = MYTYPE::NIL;

            using object_t = std::variant<double, null_t, std::string, bool>;
        }

        struct PrintVisitor
        {
            void operator()(double value) const { std::cout << value; }
            void operator()(const std::string& value) const { std::cout << value << "\n"; }
            void operator()(bool value) const { std::cout << std::boolalpha << value; }
            void operator()(null_t value) const
            {
                std::cout << "MYTYPE::NIL";
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

    }
}