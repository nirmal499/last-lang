#include <lexer/lexer.hpp>

namespace lang
{
    std::pair<std::vector<lang::Token>, std::vector<std::string>> Lexer::tokenize(std::string&& source)
    {   
        /* Initialize */
        m_source = std::move(source);
        m_source_size = m_source.size();

        m_current = 0;
        m_start = 0;
        m_line = 1;
        
        /* It is important because we are moving from this class to outside at the end of tokenize function */
        m_tokens = std::vector<lang::Token>();
        m_errors = std::vector<std::string>();
        
        while(!this->is_at_end())
        {
            m_start = m_current;

            /* 
                scan_token() is called for each character
                and it adds token to the m_tokens using the
                m_start and m_current

                After adding token each time we set m_start to point to the next lexeme
            */
            this->scan_token();
        }

        m_tokens.emplace_back(lang::Token{lang::TokenType::MYEOF, "", lang::util::null, m_line});

        return std::make_pair(
            std::move(m_tokens),
            std::move(m_errors)
        );
    }

    void Lexer::scan_token()
    {   
        /* advance() is for input and add_token() is for output */

        char ch = this->advance();
        switch(ch)
        {
            case '(': this->add_token(TokenType::LEFT_PAREN); break;
            case ')': this->add_token(TokenType::RIGHT_PAREN); break;
            case '{': this->add_token(TokenType::LEFT_BRACE); break;
            case '}': this->add_token(TokenType::RIGHT_BRACE); break;
            case ',': this->add_token(TokenType::COMMA); break;
            case '.': this->add_token(TokenType::DOT); break;
            case '-': this->add_token(TokenType::MINUS); break;
            case '+': this->add_token(TokenType::PLUS); break;
            case ';': this->add_token(TokenType::SEMICOLON); break;
            case '*': this->add_token(TokenType::STAR); break;
            case '!':
                this->add_token(this->match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
                break;
            case '=':
                this->add_token(this->match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
                break;
            case '<':
                this->add_token(this->match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
                break;
            case '>':
                this->add_token(this->match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
                break;
            case '/':
                if(this->match('/'))
                {
                    /* A comment goes until the end of the line */
                    while(this->peek() != '\n' && !this->is_at_end())
                    {
                        this->advance();
                    }
                }
                else
                {
                    this->add_token(TokenType::SLASH);
                }
                break;
            case ' ':
            case '\r':
            case '\t':
                /* 
                    Ignore whitespace
                    When encountering whitespace, we simply go back to the beginning of the scan loop
                    We dumped the returned consumed character by advance()
                */
                break;
            case '\n':
                m_line++;
                break;
            case '"': this->read_string_literal(); break;
            default:
                if(this->is_digit(ch))
                {
                    this->read_number_literal();
                }
                else if(this->is_aplha_numeric(ch))
                {
                    /* 
                        We any lexeme starting with a letter or underscore is an
                        identifier.
                    */
                    this->read_identifier();
                }
                else
                {
                    /* Note that the erroneous character is still consumed by earlier call to advance() */
                    this->generate_error(m_line, "Unexpected character.");
                }
                break;
        }
    }

    bool Lexer::is_digit(char c)
    {
        return (c >= '0' && c <= '9');
    }

    bool Lexer::is_alpha(char c)
    {
        return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
    }

    bool Lexer::is_aplha_numeric(char c)
    {
        return is_alpha(c) || is_digit(c);
    }

    void Lexer::read_identifier()
    {
        while(this->is_aplha_numeric(this->peek()))
        {
            this->advance();
        }

        int length = m_current - m_start;
        std::string text = m_source.substr(m_start, length);
        TokenType type{};

        auto it = m_keywords.find(text);
        if(it != m_keywords.end())
        {
            type = it->second;
        }
        else
        {
            type = TokenType::IDENTIFIER;
        }
        
        this->add_token(type);
    }

    /*
        A number literal is a series of digits optionally followed by a . and one or more trailing digits :- 1234, 12.34

        We don’t allow a leading or trailing decimal point, so these are both invalid:- .1234, 1234.
    */
    void Lexer::read_number_literal()
    {
        while(this->is_digit(this->peek()))
        {
            /* m_current is changed to point to next character but the m_start is at the start of the number lexeme */
            this->advance();
        }

        /* Look for a fractional part */
        if(this->peek() == '.' && this->is_digit(this->peekNext()))
        {
            /* Consume the "." */
            while(this->is_digit(this->peek()))
            {
                this->advance();
            }
        }

        /*
            We consume as many digits as we find for the integer part of the literal. Then
            we look for a fractional part, which is a decimal point (.) followed by at least
            one digit. If we do have a fractional part, again, we consume as many digits as
            we can find.

            Looking past the decimal point requires a second character of lookahead since
            we don’t want to consume the . until we’re sure there is a digit after it
        */


        int length = m_current - m_start;
        this->add_token(TokenType::NUMBER, std::stod(m_source.substr(m_start, length)));
    }

    void Lexer::read_string_literal()
    {
        /*
            We consume characters until we hit the closing " that ends
            the string. We also handle running out of input before the string literal
            is closed and report an error for that.
        */
        while(this->peek() != '"' && !this->is_at_end())
        {   
            /* We support multi-line string literals */
            if(this->peek() == '\n')
            {
                m_line++;
            }

            this->advance();
        }

        if(this->is_at_end())
        {
            this->generate_error(m_line, "Unterminated string");
            return;
        }

        /* Consume the closing " */
        this->advance();

        /* Trim the starting quote and ending quote */
        int length = (m_current - 1) - (m_start + 1);
        std::string value = m_source.substr(m_start + 1, length);
        this->add_token(TokenType::STRING, value);
    }

    /* It consumes the next character in the source file and returns it */
    char Lexer::advance()
    {
        return m_source.at(m_current++);
    }

    /* It is like conditional advance(). It only consumes the current character if it's what we're looking for. */
    bool Lexer::match(char expected)
    {
        if(this->is_at_end())
        {
            return false;
        }

        if(m_source.at(m_current) != expected)
        {
            return false;
        }

        m_current++;

        return true;
    }

    /* 
        It's sort of like advance(), but doesn't consume the character. This is called lookahead
        At any moment the m_current points to the character which is under examination
        So, peek() gives us a peek at what m_current is pointing to
    */
    char Lexer::peek()
    {
        if(this->is_at_end())
        {
            return '\0';
        }

        return m_source.at(m_current);
    }

    char Lexer::peekNext()
    {
        if(m_current + 1 >= m_source_size)
        {
            return '\0';
        }

        return m_source.at(m_current + 1);
    }

    void Lexer::add_token(TokenType type)
    {
        this->add_token(type, lang::util::null);
    }

    void Lexer::add_token(TokenType type, lang::util::object_t literal)
    {
        int length = m_current - m_start;
        std::string extracted_lexeme = m_source.substr(m_start, length);

        m_tokens.emplace_back(Token{type, extracted_lexeme, literal, m_line});
    }

    /* It returns true if current reaches end of file */
    bool Lexer::is_at_end()
    {
        return m_current >= m_source_size;
    }

    void Lexer::generate_error(int line, std::string message)
    {
        std::stringstream buffer;
        buffer << "[line " << line << "] Error : " << message << "\n";

        m_errors.emplace_back(buffer.str());
    }
}