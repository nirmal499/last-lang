#pragma once

#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <sstream>
#include <unordered_map>

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

            using object_t = std::variant<double, null_t, std::string>;
        }

        struct PrintVisitor
        {
            void operator()(double value) const { std::cout << value; }
            void operator()(const std::string& value) const { std::cout << value; }
            void operator()(null_t value) const
            {
                std::cout << "MYTYPE::NIL";
            }
        };
    }
}