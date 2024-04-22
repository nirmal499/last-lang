#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace ast
    {
        template<typename T>
        struct BaseVisitor;

        /* It is a base class */
        template<typename T>
        struct Expression
        {
            virtual T accept(BaseVisitor<T>& visitor) = 0;
        };

        template<typename T>
        struct BinaryExpression: public Expression<T>
        {

            /* Expression "left" has to outlive the BinaryExpression */
            Expression<T>& left;
            lang::Token op;
            Expression<T>& right;

            BinaryExpression(Expression<T>& left, const lang::Token& op, Expression<T>& right)
                : left(left), right(right), op(op)
            {}

            T accept(BaseVisitor<T>& visitor) override
            {
                return visitor.visit(*this);
            }
        };

        template<typename T>
        struct GroupingExpression: public Expression<T>
        {

            Expression<T>& expr;

            GroupingExpression(Expression<T>& expression)
                : expr(expression)
            {}

            T accept(BaseVisitor<T>& visitor) override
            {
                return visitor.visit(*this);
            }
        };

        template<typename T>
        struct LiteralExpression: public Expression<T>
        {

            lang::util::object_t value;

            LiteralExpression(const lang::util::object_t& value)
                : value(value)
            {}

            T accept(BaseVisitor<T>& visitor) override
            {
                return visitor.visit(*this);
            }
        };

        template<typename T>
        struct UnaryExpression: public Expression<T>
        {

            lang::Token op;
            Expression<T>& right;

            UnaryExpression(const lang::Token& op, Expression<T>& right)
                : op(op), right(right)
            {}

            T accept(BaseVisitor<T>& visitor) override
            {
                return visitor.visit(*this);
            }
        };

        template<typename T>
        struct BaseVisitor
        {
            virtual T visit(BinaryExpression<T>& expression) = 0;
            virtual T visit(GroupingExpression<T>& expression) = 0;
            virtual T visit(LiteralExpression<T>& expression) = 0;
            virtual T visit(UnaryExpression<T>& expression) = 0;
        };
    }
}