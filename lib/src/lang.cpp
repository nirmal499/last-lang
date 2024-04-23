#include <lang/lang.hpp>

namespace lang
{
    void Lang::run_source_code(const char* absolute_path_of_source_code)
    {
        std::ifstream file(absolute_path_of_source_code);

        if(!file.is_open())
        {
            throw std::runtime_error("Error opening the file");
        }

        file.seekg(0, std::ios::end); /* Seek to end of file */
        auto file_size = file.tellg();

        std::string file_content;
        file_content.resize(file_size);

        file.seekg(0, std::ios::beg);
        file.read(file_content.data(), file_size);

        file.close();
        
        /* run the file contents */
        this->run(std::move(file_content));
    }
    
    void Lang::run_repl()
    {
        const char* PROMPT = ">> ";
        std::string line{};

        while(true)
        {
            std::cout << PROMPT;
            std::getline(std::cin, line);

            if(line.empty())
            {
                break;
            }

            this->run(std::move(line));
            std::cout << "\n\n";
            line = std::string{};
        }
    }

    /*
    void Lang::run(std::string&& source)
    {
        std::pair<std::vector<lang::Token>, std::vector<std::string>> tokens_and_errors = m_lexer->tokenize(std::move(source));
        
        if(tokens_and_errors.second.size() > 0)
        {
            std::cout << "ERROR FOUND DURING TOKENIZATION:\n";
            for(const auto& error: tokens_and_errors.second)
            {
                std::cout << error << "\n";
            }
            
            return;
        }

        for(const auto& token: tokens_and_errors.first)
        {
            std::cout << token << "\n";
        }
    }
    */

    void Lang::run(std::string&& source)
    {
        /********************************************************************************************************/
        auto [tokens, tokenization_errors] = m_lexer->tokenize(std::move(source));
        
        if(tokenization_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING TOKENIZATION:\n";
            for(const auto& error: tokenization_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
        /********************************************************************************************************/

        auto tokens_copy = tokens;

        /********************************************************************************************************/
        auto [ast_expression_raw_ptr_for_ast_printer, parsing_errors_for_ast_printer] = m_parser_ast_printer->parse(std::move(tokens));

        if(ast_expression_raw_ptr_for_ast_printer == nullptr || parsing_errors_for_ast_printer.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING PARSING FOR AST_PRINTER:\n";
            for(const auto& error: parsing_errors_for_ast_printer)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
        
        std::cout << "\nThe AST is: \n\n";
        std::cout << m_ast_printer->print(ast_expression_raw_ptr_for_ast_printer);
        /********************************************************************************************************/

        /********************************************************************************************************/
        auto [ast_expression_raw_ptr_for_ast_interpreter, parsing_errors_for_ast_interpreter] = m_parser_ast_interpreter->parse(std::move(tokens_copy));

        if(ast_expression_raw_ptr_for_ast_interpreter == nullptr || parsing_errors_for_ast_interpreter.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING PARSING FOR AST_INTERPRETER:\n";
            for(const auto& error: parsing_errors_for_ast_interpreter)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
        /********************************************************************************************************/

        auto [result_object, evaluation_errors] = m_ast_interpreter->interpret(ast_expression_raw_ptr_for_ast_interpreter);

        if(evaluation_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING EVALUATION:\n";
            for(const auto& error: evaluation_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }

        std::cout << "\nEvaluated Result is :\n\n";
        std::visit(lang::util::PrintVisitor{}, result_object);
        std::cout << "\n\n";
    }
}