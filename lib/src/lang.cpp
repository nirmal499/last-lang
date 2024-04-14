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
            line = std::string{};
        }
    }

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
}