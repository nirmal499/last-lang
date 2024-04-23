#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace interpret
    {
        struct BinaryExpression;
        struct GroupingExpression;
        struct LiteralExpression;
        struct UnaryExpression;

        struct BaseVisitor
        {
            virtual lang::util::object_t visit(BinaryExpression* expression) = 0;
            virtual lang::util::object_t visit(GroupingExpression* expression) = 0;
            virtual lang::util::object_t visit(LiteralExpression* expression) = 0;
            virtual lang::util::object_t visit(UnaryExpression* expression) = 0;
        };

        struct Expression
        {
            virtual lang::util::object_t accept(BaseVisitor* visitor) = 0;
        };

        struct BinaryExpression: public Expression
        {
            Expression* left;
            lang::Token op;
            Expression* right;

            BinaryExpression(Expression* left, const lang::Token& op, Expression* right);

            lang::util::object_t accept(BaseVisitor* visitor) override;
        };

        struct GroupingExpression: public Expression
        {
            Expression* expr;

            GroupingExpression(Expression* expression);

            lang::util::object_t accept(BaseVisitor* visitor) override;
        };

        struct LiteralExpression: public Expression
        {
            lang::util::object_t value;

            LiteralExpression(const lang::util::object_t& value);

            lang::util::object_t accept(BaseVisitor* visitor) override;
        };

        struct UnaryExpression: public Expression
        {
            lang::Token op;
            Expression* value;

            UnaryExpression(const lang::Token& op, Expression* value);

            lang::util::object_t accept(BaseVisitor* visitor) override;
        };


    }
}