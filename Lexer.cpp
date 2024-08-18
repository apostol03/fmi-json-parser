#include "Lexer.h"

Lexer::Lexer(const std::string &input) : input(input), pos(0), line(1), column(1) {}

size_t Lexer::getLine()
{
    return line;
}

size_t Lexer::getColumn()
{
    return column;
}

Token Lexer::nextToken()
{
    skipWhitespace();

    if (pos >= input.length())
    {
        return {TokenType::END, ""};
    }

    char curr = input[pos];
    switch (curr)
    {
    case '{':
        pos++;
        return {TokenType::LEFT_BRACE, "{"};
    case '}':
        pos++;
        return {TokenType::RIGHT_BRACE, "}"};
    case '[':
        pos++;
        return {TokenType::LEFT_BRACKET, "["};
    case ']':
        pos++;
        return {TokenType::RIGHT_BRACKET, "]"};
    case ',':
        pos++;
        return {TokenType::COMMA, ","};
    case ':':
        pos++;
        return {TokenType::COLON, ":"};
    case '"':
        return parseString();
    case 't':
    case 'f':
    case 'n':
        return parseKeyword();
    default:
        if (isdigit(curr) || curr == '-')
        {
            return parseNumber();
        }
        throw std::runtime_error("Unexpected character at line " + std::to_string(line) + ", column " + std::to_string(column));
    }
}

void Lexer::resetPos()
{
    pos = 0;
}

void Lexer::advance()
{
    if (input[pos] == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }
    pos++;
}

void Lexer::skipWhitespace()
{
    while (pos < input.size() && isspace(input[pos]))
    {
        advance();
    }
}

Token Lexer::parseString()
{
    size_t start = pos + 1;
    advance();
    while (pos < input.size() && input[pos] != '"')
    {
        advance();
    }
    if (pos >= input.size())
    {
        throw std::runtime_error("Unterminated string at line " + std::to_string(line) + ", column " + std::to_string(column));
    }
    size_t end = pos;
    advance();
    return {TokenType::STRING, input.substr(start, end - start)};
}

Token Lexer::parseNumber()
{
    size_t start = pos;
    while (pos < input.size() && (isdigit(input[pos]) || input[pos] == '.' || input[pos] == '-' || input[pos] == '+'))
    {
        advance();
    }
    return {TokenType::NUMBER, input.substr(start, pos - start)};
}

Token Lexer::parseKeyword()
{
    size_t start = pos;
    while (pos < input.size() && isalpha(input[pos]))
    {
        advance();
    }
    std::string keyword = input.substr(start, pos - start);
    if (keyword == "true")
        return {TokenType::TRUE, "true"};
    if (keyword == "false")
        return {TokenType::FALSE, "false"};
    if (keyword == "null")
        return {TokenType::NULL_TYPE, "null"};
    throw std::runtime_error("Invalid keyword '" + keyword + "' at line " + std::to_string(line) + ", column " + std::to_string(column));
}
