#include <environment/environment.hpp>

namespace lang
{
    namespace env
    {
        Environment::Environment(Environment* enclosing): m_enclosing(enclosing) {}

        Environment::~Environment(){}
        lang::util::object_t Environment::get(const lang::Token& name)
        {   
            auto it = m_values.find(name.m_lexeme);
            if(it != m_values.end())
            {
                return it->second;
            }

            if(m_enclosing != nullptr && m_enclosing != this)
            {
                return m_enclosing->get(name);
            }

            throw std::runtime_error("Undefined variable '" + name.m_lexeme + "'.");
        }

        void Environment::assign(const lang::Token& name, const lang::util::object_t& value)
        {   
            auto it = m_values.find(name.m_lexeme);
            if(it != m_values.end())
            {
                m_values[name.m_lexeme] = value;
                return;
            }

            if(m_enclosing != nullptr)
            {
                m_enclosing->assign(name, value);
                return;
            }

            throw std::runtime_error("Undefined variable '" + name.m_lexeme + "'.");
        }

        void Environment::define(const std::string& name, const lang::util::object_t& value)
        {
            m_values[name] = value;
        }
    }
}