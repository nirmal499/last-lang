#include <ast/ast_printer.hpp>

std::string ASTPrinter::print(lang::ast::Expression* expression)
{
    return expression->accept(this);
}

std::string ASTPrinter::parenthesize(std::string name, lang::ast::Expression* one)
{
    std::stringstream buffer;
    buffer << "(" << name;

    buffer << " " << one->accept(this);

    buffer << ")";

    return buffer.str();
}

std::string ASTPrinter::parenthesize(std::string name, lang::ast::Expression* one, lang::ast::Expression* two)
{
    std::stringstream buffer;
    buffer << "(" << name;

    buffer << " " << one->accept(this);
    buffer << " " << two->accept(this);

    buffer << ")";

    return buffer.str();
}

std::string ASTPrinter::visit(lang::ast::BinaryExpression* expression)
{
    return this->parenthesize(expression->op.m_lexeme, expression->left, expression->right);
}

std::string ASTPrinter::visit(lang::ast::GroupingExpression* expression)
{
    return this->parenthesize("group", expression->expr);
}

std::string ASTPrinter::visit(lang::ast::LiteralExpression* expression)
{
    std::string value;

    if(auto* data = std::get_if<lang::util::null_t>(&expression->value))
    {
        return "nil";
    }
    else if(auto* data = std::get_if<double>(&expression->value))
    {
        double value_of_type_double = std::get<double>(expression->value);
        value = std::to_string(value_of_type_double);

    }
    else if(auto* data = std::get_if<std::string>(&expression->value))
    {
        value = std::get<std::string>(expression->value);
    }

    return value;
}


std::string ASTPrinter::visit(lang::ast::UnaryExpression* expression)
{
    return this->parenthesize(expression->op.m_lexeme, expression->value);
}