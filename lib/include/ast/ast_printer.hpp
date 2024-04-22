#pragma once

#include <types/types.hpp>
#include <ast/ast.hpp>

struct ASTPrinter: public lang::ast::BaseVisitor<std::string>
{
    using T = std::string;

    T print(lang::ast::Expression<T>& expression)
    {
        return expression.accept(*this);
    }

    std::string parenthesize(std::string name, lang::ast::Expression<T>& one)
    {
        std::stringstream buffer;
        buffer << "(" << name;
 
        buffer << " " << one.accept(*this);

        buffer << ")";

        return buffer.str();
    }

    std::string parenthesize(std::string name, lang::ast::Expression<T>& one, lang::ast::Expression<T>& two)
    {
        std::stringstream buffer;
        buffer << "(" << name;
 
        buffer << " " << one.accept(*this);
        buffer << " " << two.accept(*this);

        buffer << ")";

        return buffer.str();
    }

    T visit(lang::ast::BinaryExpression<T>& expression) override
    {
        return this->parenthesize(expression.op.m_lexeme, expression.left, expression.right);
    }
    T visit(lang::ast::GroupingExpression<T>& expression) override
    {
        return this->parenthesize("group", expression.expr);
    }
    T visit(lang::ast::LiteralExpression<T>& expression) override
    {
        std::string value;

        if(auto* data = std::get_if<lang::util::null_t>(&expression.value))
        {
            return "nil";
        }
        else if(auto* data = std::get_if<double>(&expression.value))
        {
            double value_of_type_double = std::get<double>(expression.value);
            value = std::to_string(value_of_type_double);

        }
        else if(auto* data = std::get_if<std::string>(&expression.value))
        {
            value = std::get<std::string>(expression.value);
        }

        return value;
    }
    T visit(lang::ast::UnaryExpression<T>& expression) override
    {
        return this->parenthesize(expression.op.m_lexeme, expression.right);
    }

};