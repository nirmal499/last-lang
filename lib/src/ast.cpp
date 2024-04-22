#include <ast/ast.hpp>

namespace lang
{
    namespace ast
    {   
        BinaryExpression::BinaryExpression(Expression* left, const lang::Token& op, Expression* right)
                : left(std::move(left)), right(std::move(right)), op(op)
        {}
        
        std::string BinaryExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }

        GroupingExpression::GroupingExpression(Expression* expression)
                : expr(std::move(expression))
        {}

        std::string GroupingExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }

        LiteralExpression::LiteralExpression(const lang::util::object_t& value)
                : value(value)
        {}

        std::string LiteralExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }

        UnaryExpression::UnaryExpression(const lang::Token& op, Expression* value)
                : op(op), value(std::move(value))
        {}

        std::string UnaryExpression::accept(BaseVisitor* visitor)
        {
            return visitor->visit(this);
        }
    }
}