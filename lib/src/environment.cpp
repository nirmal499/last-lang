#include <environment/environment.hpp>

namespace lang
{
    namespace env
    {
        lang::util::object_t Environment::get(const lang::Token& name)
        {   
            auto it = m_values.find(name.m_lexeme);
            if(it != m_values.end())
            {
                return it->second;
            }

            throw std::runtime_error("Undefined variable '" + name.m_lexeme + "'.");
        }

        void Environment::define(const std::string& name, const lang::util::object_t& value)
        {
            m_values[name] = value;
        }
    }
}