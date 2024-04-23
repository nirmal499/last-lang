#pragma once

#include <types/types.hpp>
#include <ast/ast.hpp>

namespace lang
{
    template<typename T>
    class Interpreter: public lang::ast::BaseVisitor<T>
    {
        public:
            Interpreter(){}

            std::pair<lang::util::object_t, std::vector<std::string>> interpret(lang::ast::Expression<T>* expression);

        private:
            lang::util::object_t evaluate(lang::ast::Expression<T>* expression);

            /*************************************************************************************************************/
            T visit(lang::ast::BinaryExpression<T>* expression) override;

            T visit(lang::ast::GroupingExpression<T>* expression) override;

            T visit(lang::ast::LiteralExpression<T>* expression) override;

            T visit(lang::ast::UnaryExpression<T>* expression) override;
            /*************************************************************************************************************/

            lang::util::object_t is_truthy(const lang::util::object_t& object);

            bool is_equal(const lang::util::object_t& object_A, const lang::util::object_t& object_B);

            void generate_error(int line, std::string message);

        private:
            std::vector<std::string> m_errors;
    };
}