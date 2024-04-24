#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace env
    {
        class Environment
        {
            public:
                lang::util::object_t get(const lang::Token& name);
                
                void define(const std::string& name, const lang::util::object_t& value);

                void assign(const lang::Token& name, lang::util::object_t value);
            private:
                std::unordered_map<std::string, lang::util::object_t> m_values;
        };
    }
}