#pragma once

#include <types/types.hpp>
#include <ast/ast.hpp>

struct ASTPrinter: public lang::ast::BaseVisitor
{
    std::string print(lang::ast::Expression* expression);

    std::string parenthesize(std::string name, lang::ast::Expression* one);

    std::string parenthesize(std::string name, lang::ast::Expression* one, lang::ast::Expression* two);

    std::string visit(lang::ast::BinaryExpression* expression) override;

    std::string visit(lang::ast::GroupingExpression* expression) override;

    std::string visit(lang::ast::LiteralExpression* expression) override;

    std::string visit(lang::ast::UnaryExpression* expression) override;
};