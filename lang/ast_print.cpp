#include <ast/ast.hpp>
#include <ast/ast_printer.hpp>

int main()
{
    ASTPrinter ast_printer;

    lang::util::object_t value1 = 123.0;
    lang::util::object_t value2 = 45.67;

    auto literal_expr_with_value1 = std::make_unique<lang::ast::LiteralExpression>(value1);
    auto literal_expr_with_value2 = std::make_unique<lang::ast::LiteralExpression>(value2);

    lang::Token star_token = lang::Token{lang::TokenType::STAR, "*", lang::util::null, 1};
    lang::Token minus_token = lang::Token{lang::TokenType::MINUS, "-", lang::util::null, 1};

    auto unary_expr = std::make_unique<lang::ast::UnaryExpression>(minus_token, literal_expr_with_value1.get());
    auto group_expr = std::make_unique<lang::ast::GroupingExpression>(literal_expr_with_value2.get());

    auto binary_expr = std::make_unique<lang::ast::BinaryExpression>
    (
        unary_expr.get(),
        star_token,
        group_expr.get()
    );

    std::cout << ast_printer.print(binary_expr.get());

    std::cout << "\n";
    
    return 0;
}