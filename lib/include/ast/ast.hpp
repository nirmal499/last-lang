#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace ast
    {   
        template<typename T>
        struct BinaryExpression;

        template<typename T>
        struct GroupingExpression;

        template<typename T>
        struct LiteralExpression;

        template<typename T>
        struct UnaryExpression;

        template<typename T>
        struct BaseVisitor
        {
            virtual T visit(BinaryExpression<T>* expression) = 0;
            virtual T visit(GroupingExpression<T>* expression) = 0;
            virtual T visit(LiteralExpression<T>* expression) = 0;
            virtual T visit(UnaryExpression<T>* expression) = 0;
        };

        template<typename T>
        struct Expression
        {
            virtual T accept(BaseVisitor<T>* visitor) = 0;
        };

        template<typename T>
        struct BinaryExpression: public Expression<T>
        {
            Expression<T>* left;
            lang::Token op;
            Expression<T>* right;

            BinaryExpression(Expression<T>* left, const lang::Token& op, Expression<T>* right)
                : left(std::move(left)), right(std::move(right)), op(op)
            {}

            T accept(BaseVisitor<T>* visitor) override
            {
                return visitor->visit(this);
            }
        };

        template<typename T>
        struct GroupingExpression: public Expression<T>
        {
            Expression<T>* expr;

            GroupingExpression(Expression<T>* expression)
                : expr(std::move(expression))
            {}

            T accept(BaseVisitor<T>* visitor) override
            {
                return visitor->visit(this);
            }
        };

        template<typename T>
        struct LiteralExpression: public Expression<T>
        {
            lang::util::object_t value;

            LiteralExpression(const lang::util::object_t& value)
                : value(value)
            {}

            T accept(BaseVisitor<T>* visitor) override
            {
                return visitor->visit(this);
            }
        };

        template<typename T>
        struct UnaryExpression: public Expression<T>
        {
            lang::Token op;
            Expression<T>* value;

            UnaryExpression(const lang::Token& op, Expression<T>* value)
                : op(op), value(std::move(value))
            {}

            T accept(BaseVisitor<T>* visitor) override
            {
                return visitor->visit(this);
            }
        };
    }
}