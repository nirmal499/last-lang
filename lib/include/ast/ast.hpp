#pragma once

#include <types/types.hpp>
#include <token/token.hpp>

namespace lang
{
    namespace ast
    {   

        /**********************************************************************************************************************8*/
        struct ExpressionStatement;
        struct PrintStatement;
        struct VarStatement;
        struct BlockStatement;
        struct IfStatement;
        
        struct BaseVisitorForStatement
        {
            virtual void visit(ExpressionStatement* statement) = 0;
            virtual void visit(PrintStatement* statement) = 0;
            virtual void visit(VarStatement* statement) = 0;
            virtual void visit(BlockStatement* statement) = 0;
            virtual void visit(IfStatement* statement) = 0;
        };

        struct Statement
        {
            virtual void accept(BaseVisitorForStatement* visitor) = 0;
        };
        /**********************************************************************************************************************8*/


        struct BinaryExpression;
        struct GroupingExpression;
        struct LiteralExpression;
        struct UnaryExpression;
        struct VariableExpression;
        struct AssignmentExpression;
        struct LogicalExpression;

        struct BaseVisitorForExpression
        {
            virtual lang::util::object_t visit(BinaryExpression* expression) = 0;
            virtual lang::util::object_t visit(GroupingExpression* expression) = 0;
            virtual lang::util::object_t visit(LiteralExpression* expression) = 0;
            virtual lang::util::object_t visit(UnaryExpression* expression) = 0;
            virtual lang::util::object_t visit(VariableExpression* expression) = 0;
            virtual lang::util::object_t visit(AssignmentExpression* expression) = 0;
            virtual lang::util::object_t visit(LogicalExpression* expression) = 0;
        };

        struct Expression
        {
            virtual lang::util::object_t accept(BaseVisitorForExpression* visitor) = 0;
        };

        /**********************************************************************************************************************8*/
        struct ExpressionStatement: public Statement
        {
            Expression* expr;

            ExpressionStatement(Expression* expr)
                : expr(std::move(expr))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct PrintStatement: public Statement
        {
            Expression* expr;

            PrintStatement(Expression* expr)
                : expr(std::move(expr))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct VarStatement: public Statement
        {
            lang::Token name;
            Expression* initializer;

            VarStatement(const lang::Token& name, Expression* initializer)
                : name(name), initializer(std::move(initializer))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct BlockStatement: public Statement
        {
            std::vector<Statement*> statements;

            BlockStatement(std::vector<Statement*>&& statements)
                : statements(std::move(statements))
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct IfStatement: public Statement
        {
            Expression* condition;
            Statement* thenBranch;
            Statement* elseBranch;

            IfStatement(Expression* condition, Statement* thenBranch, Statement* elseBranch)
                : condition(condition), thenBranch(thenBranch), elseBranch(elseBranch)
            {}

            void accept(BaseVisitorForStatement* visitor) override
            {
                return visitor->visit(this);
            }
        };
        /**********************************************************************************************************************8*/


        struct BinaryExpression: public Expression
        {
            Expression* left;
            lang::Token op;
            Expression* right;

            BinaryExpression(Expression* left, const lang::Token& op, Expression* right)
                : left(std::move(left)), right(std::move(right)), op(op)
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct GroupingExpression: public Expression
        {
            Expression* expr;

            GroupingExpression(Expression* expression)
                : expr(std::move(expression))
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct LiteralExpression: public Expression
        {
            lang::util::object_t value;

            LiteralExpression(const lang::util::object_t& value)
                : value(value)
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct UnaryExpression: public Expression
        {
            lang::Token op;
            Expression* value;

            UnaryExpression(const lang::Token& op, Expression* value)
                : op(op), value(std::move(value))
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct VariableExpression: public Expression
        {
            lang::Token name;

            VariableExpression(const lang::Token& name)
                : name(name)
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct AssignmentExpression: public Expression
        {
            lang::Token name;
            Expression* value;


            AssignmentExpression(const lang::Token& name, Expression* value)
                : name(name), value(value)
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };

        struct LogicalExpression: public Expression
        {
            Expression* left;
            lang::Token op;
            Expression* right;


            LogicalExpression(Expression* left, const lang::Token& op, Expression* right)
                : left(left), op(op), right(right)
            {}

            lang::util::object_t accept(BaseVisitorForExpression* visitor) override
            {
                return visitor->visit(this);
            }
        };
    }
}