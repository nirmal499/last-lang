#pragma once

#include <types/types.hpp>
#include <interpreter/interpret.hpp>

namespace lang
{
    class Interpreter: public lang::interpret::BaseVisitor
    {
        public:
            Interpreter(){}

            std::pair<lang::util::object_t, std::vector<std::string>> interpret(lang::interpret::Expression* expression);

        private:
            lang::util::object_t evaluate(lang::interpret::Expression* expression);

            lang::util::object_t visit(lang::interpret::BinaryExpression* expression) override;

            lang::util::object_t visit(lang::interpret::GroupingExpression* expression) override;

            lang::util::object_t visit(lang::interpret::LiteralExpression* expression) override;

            lang::util::object_t visit(lang::interpret::UnaryExpression* expression) override;

            lang::util::object_t is_truthy(const lang::util::object_t& object);

            bool is_equal(const lang::util::object_t& object_A, const lang::util::object_t& object_B);

            void generate_error(int line, std::string message);

        private:
            std::vector<std::string> m_errors;
    };
}