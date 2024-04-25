#pragma once

#include <types/types.hpp>
#include <ast/ast.hpp>
#include <environment/environment.hpp>

namespace lang
{
    class Interpreter: public lang::ast::BaseVisitorForExpression, public lang::ast::BaseVisitorForStatement
    {
        public:
            Interpreter(){}

            std::vector<std::string> interpret(std::vector<lang::ast::Statement*>&& statements);

        private:
            lang::util::object_t evaluate(lang::ast::Expression* expression);
            void execute(lang::ast::Statement* statement);
            void execute_block(const std::vector<lang::ast::Statement*> stmts, lang::env::Environment* env);

            /*************************************************************************************************************/
            lang::util::object_t visit(lang::ast::BinaryExpression* expression) override;

            lang::util::object_t visit(lang::ast::GroupingExpression* expression) override;

            lang::util::object_t visit(lang::ast::LiteralExpression* expression) override;

            lang::util::object_t visit(lang::ast::UnaryExpression* expression) override;
            
            lang::util::object_t visit(lang::ast::VariableExpression* expression) override;

            lang::util::object_t visit(lang::ast::AssignmentExpression* expression) override;

            lang::util::object_t visit(lang::ast::LogicalExpression* expression) override;

            /*************************************************************************************************************/

            void visit(lang::ast::ExpressionStatement* statement) override;
            
            void visit(lang::ast::PrintStatement* statement) override;

            void visit(lang::ast::VarStatement* statement) override;
            
            void visit(lang::ast::BlockStatement* statement) override;

            void visit(lang::ast::IfStatement* statement) override;

            /*************************************************************************************************************/

            lang::util::object_t is_truthy(const lang::util::object_t& object);

            bool is_equal(const lang::util::object_t& object_A, const lang::util::object_t& object_B);

            void generate_error(int line, std::string message);

        private:
            std::vector<std::string> m_errors;
            lang::env::Environment* m_environment = nullptr;

            std::vector<std::unique_ptr<lang::env::Environment>> m_temp_envs;
    };
}