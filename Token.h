#ifndef TOKEN_H
#define TOKEN_H

#include <string>

/**
 * Enum representing the type of a token in JSON.
 */
enum class TokenType
{
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    COMMA,
    COLON,
    STRING,
    NUMBER,
    TRUE,
    FALSE,
    NULL_TYPE,
    END
};

/**
 * Structure representing a token in JSON.
 */
struct Token
{
    TokenType type;
    std::string value;

    Token(TokenType type = TokenType::END, const std::string &value = "")
        : type(type), value(value) {}
};

#endif