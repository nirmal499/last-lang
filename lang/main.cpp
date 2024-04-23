#include <iostream>
#include <filesystem>
#include <stdexcept>

#include <lang/lang.hpp>

/* $ ./main.out file  :- For this "argc" is 2 */
int main(int argc, const char* argv[])
{
    try
    {
        lang::Lang application;

        if(argc > 2 || argc <= 1)
        {
            std::cout << "Usage: last [absolute_path_to_the_source_code_file]\n";
            return EXIT_FAILURE;
        }
        else if(argc == 2)
        {
            if(!std::filesystem::exists(argv[1]))
            {
                std::cout << "Provided file does not exists\n";
                return EXIT_FAILURE;
            }

            application.run_source_code(argv[1]);
            return EXIT_SUCCESS;
        }

    }catch(const std::exception& e)
    {
        std::cerr << e.what() << "\n";
    }

    return EXIT_FAILURE;
}