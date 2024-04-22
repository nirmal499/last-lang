#include <ast/ast.hpp>
#include <ast/ast_printer.hpp>

int main()
{
    ASTPrinter ast_printer;

    lang::util::object_t value1 = 123.0;
    lang::util::object_t value2 = 45.67;

    lang::ast::LiteralExpression<std::string> literal_expr_with_value1 = lang::ast::LiteralExpression<std::string>{value1};
    lang::ast::LiteralExpression<std::string> literal_expr_with_value2 = lang::ast::LiteralExpression<std::string>{value2};

    lang::Token star_token = lang::Token{lang::TokenType::STAR, "*", lang::util::null, 1};
    lang::Token minus_token = lang::Token{lang::TokenType::MINUS, "-", lang::util::null, 1};

    lang::ast::UnaryExpression<std::string> unary_expr = lang::ast::UnaryExpression<std::string>{minus_token, literal_expr_with_value1};

    lang::ast::GroupingExpression<std::string> group_expr = lang::ast::GroupingExpression{literal_expr_with_value2};

    lang::ast::BinaryExpression<std::string> binary_expr = lang::ast::BinaryExpression<std::string>{unary_expr, star_token, group_expr};

    std::cout << ast_printer.print(binary_expr);

    std::cout << "\n";
}