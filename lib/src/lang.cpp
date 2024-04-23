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
        auto [statements, parsing_errors] = m_parser->parse(std::move(tokens));

        if(statements.size() == 0 || parsing_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING PARSING:\n";
            for(const auto& error: parsing_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
        
        /********************************************************************************************************/

        auto evaluation_errors = m_interpreter->interpret(std::move(statements));

        if(evaluation_errors.size() > 0)
        {
            std::cout << "\nERROR FOUND DURING EVALUATION:\n";
            for(const auto& error: evaluation_errors)
            {
                std::cout << error << "\n";
            }
            
            return;
        }
    }
}