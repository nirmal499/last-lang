#include <ast/ast_printer.hpp>

namespace lang
{
    namespace ast
    {
        template<typename T>
        std::string ASTPrinter<T>::print(lang::ast::Expression<T>* expression)
        {
            return expression->accept(this);
        }

        template<typename T>
        std::string ASTPrinter<T>::parenthesize(std::string name, lang::ast::Expression<T>* one)
        {
            std::stringstream buffer;
            buffer << "(" << name;

            buffer << " " << one->accept(this);

            buffer << ")";

            return buffer.str();
        }

        template<typename T>
        std::string ASTPrinter<T>::parenthesize(std::string name, lang::ast::Expression<T>* one, lang::ast::Expression<T>* two)
        {
            std::stringstream buffer;
            buffer << "(" << name;

            buffer << " " << one->accept(this);
            buffer << " " << two->accept(this);

            buffer << ")";

            return buffer.str();
        }

        template<typename T>
        T ASTPrinter<T>::visit(lang::ast::BinaryExpression<T>* expression)
        {
            return this->parenthesize(expression->op.m_lexeme, expression->left, expression->right);
        }

        template<typename T>
        T ASTPrinter<T>::visit(lang::ast::GroupingExpression<T>* expression)
        {
            return this->parenthesize("group", expression->expr);
        }

        template<typename T>
        T ASTPrinter<T>::visit(lang::ast::LiteralExpression<T>* expression)
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

        template<typename T>
        T ASTPrinter<T>::visit(lang::ast::UnaryExpression<T>* expression)
        {
            return this->parenthesize(expression->op.m_lexeme, expression->value);
        }

        /* https://stackoverflow.com/questions/8752837/undefined-reference-to-template-class-constructor */
        template class ASTPrinter<std::string>;
    }
}