#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace ast
    {   
        struct BinaryExpression;
        struct GroupingExpression;
        struct LiteralExpression;
        struct UnaryExpression;

        struct BaseVisitor
        {
            virtual std::string visit(BinaryExpression* expression) = 0;
            virtual std::string visit(GroupingExpression* expression) = 0;
            virtual std::string visit(LiteralExpression* expression) = 0;
            virtual std::string visit(UnaryExpression* expression) = 0;
        };

        struct Expression
        {
            virtual std::string accept(BaseVisitor* visitor) = 0;
        };

        struct BinaryExpression: public Expression
        {
            Expression* left;
            lang::Token op;
            Expression* right;

            BinaryExpression(Expression* left, const lang::Token& op, Expression* right);

            std::string accept(BaseVisitor* visitor) override;
        };

        struct GroupingExpression: public Expression
        {
            Expression* expr;

            GroupingExpression(Expression* expression);

            std::string accept(BaseVisitor* visitor) override;
        };

        struct LiteralExpression: public Expression
        {
            lang::util::object_t value;

            LiteralExpression(const lang::util::object_t& value);

            std::string accept(BaseVisitor* visitor) override;
        };

        struct UnaryExpression: public Expression
        {
            lang::Token op;
            Expression* value;

            UnaryExpression(const lang::Token& op, Expression* value);

            std::string accept(BaseVisitor* visitor) override;
        };
    }
}