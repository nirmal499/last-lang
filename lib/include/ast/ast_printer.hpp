#pragma once

#include <types/types.hpp>
#include <ast/ast.hpp>

namespace lang
{
    namespace ast
    {
        template<typename T>
        struct ASTPrinter: public lang::ast::BaseVisitor<T>
        {
            std::string print(lang::ast::Expression<T>* expression);

            std::string parenthesize(std::string name, lang::ast::Expression<T>* one);

            std::string parenthesize(std::string name, lang::ast::Expression<T>* one, lang::ast::Expression<T>* two);

            /************************************************************************************************************/
            T visit(lang::ast::BinaryExpression<T>* expression) override;

            T visit(lang::ast::GroupingExpression<T>* expression) override;

            T visit(lang::ast::LiteralExpression<T>* expression) override;

            T visit(lang::ast::UnaryExpression<T>* expression) override;
            /************************************************************************************************************/
        };
    }
}