#include <interpreter/interpret.hpp>

namespace lang
{
    namespace interpret
    {
        BinaryExpression::BinaryExpression(Expression* left, const lang::Token& op, Expression* right)
                : left(std::move(left)), right(std::move(right)), op(op)
        {}
        
        lang::util::object_t BinaryExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }

        GroupingExpression::GroupingExpression(Expression* expression)
                : expr(std::move(expression))
        {}

        lang::util::object_t GroupingExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }

        LiteralExpression::LiteralExpression(const lang::util::object_t& value)
                : value(value)
        {}

        lang::util::object_t LiteralExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }

        UnaryExpression::UnaryExpression(const lang::Token& op, Expression* value)
                : op(op), value(std::move(value))
        {}

        lang::util::object_t UnaryExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }
    }
}