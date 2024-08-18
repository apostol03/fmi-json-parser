#include "Parser.h"

Parser::Parser(const std::string &input, const std::string &currentFilePath = "") : lexer(input), currentFilePath(currentFilePath), currentToken(lexer.nextToken()), root(parseValue()) {}

JSONValue Parser::parse()
{
    return root;
}

bool Parser::validate()
{
    try
    {
        lexer.resetPos();
        currentToken = lexer.nextToken();
        validateValue();
        if (currentToken.type != TokenType::END)
        {
            throw std::runtime_error("Unexpected characters at the end of JSON input.");
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Validation error: " << e.what() << std::endl;
        return false;
    }
}

std::vector<JSONValue *> Parser::searchKey(const std::string &key) const
{
    std::vector<JSONValue *> results;
    root.searchKey(key, results);
    return results;
}

std::vector<JSONValue *> Parser::searchKey(const std::regex &pattern) const
{
    std::vector<JSONValue *> results;
    root.searchKey(pattern, results);
    return results;
}

bool Parser::contains(const std::string &value) const
{
    return containsHelper(root, value);
}

bool Parser::set(const std::string &path, const std::string &newValue)
{
    std::vector<std::string> keys = splitPath(path);
    if (keys.empty())
    {
        std::cerr << "Invalid path!" << std::endl;
        return false;
    }

    JSONValue *target = &root;
    for (size_t i = 0; i < keys.size() - 1; i++)
    {
        if (target->type != JSONValueType::OBJECT)
        {
            std::cerr << "Invalid path: " << keys[i] << " is not an object." << std::endl;
            return false;
        }

        auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == keys[i]; });
        if (it == target->objectValue.end())
        {
            std::cerr << "Path element not found: " << keys[i] << std::endl;
            return false;
        }

        target = it->value;
    }

    std::string finalKey = keys.back();
    if (target->type != JSONValueType::OBJECT)
    {
        std::cerr << "Invalid path: final element is not an object." << std::endl;
        return false;
    }

    auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                           { return kv.key == finalKey; });
    if (it == target->objectValue.end())
    {
        std::cerr << "Final path element not found: " << finalKey << std::endl;
        return false;
    }

    Lexer valueLexer(newValue);
    Token valueToken = valueLexer.nextToken();
    if (valueToken.type == TokenType::END)
    {
        std::cerr << "Invalid new value!" << std::endl;
        return false;
    }

    JSONValue newParsedValue;
    try
    {
        Parser valueParser(newValue);
        valueParser.currentToken = valueToken;
        newParsedValue = valueParser.parseValue();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Invalid new value: " << e.what() << std::endl;
        return false;
    }

    delete it->value;
    it->value = new JSONValue(newParsedValue);
    return true;
}

bool Parser::create(const std::string &path, const std::string &newValue)
{
    std::vector<std::string> keys = splitPath(path);
    if (keys.empty())
    {
        std::cerr << "Invalid path." << std::endl;
        return false;
    }

    JSONValue *target = &root;
    for (size_t i = 0; i < keys.size() - 1; ++i)
    {
        if (target->type != JSONValueType::OBJECT)
        {
            std::cerr << "Invalid path: " << keys[i] << " is not an object." << std::endl;
            return false;
        }

        auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == keys[i]; });
        if (it == target->objectValue.end())
        {
            JSONValue *newObject = new JSONValue();
            newObject->type = JSONValueType::OBJECT;
            target->objectValue.push_back(KeyValue(keys[i], newObject));
            target = newObject;
        }
        else
        {
            target = it->value;
        }
    }

    std::string finalKey = keys.back();
    if (target->type != JSONValueType::OBJECT)
    {
        std::cerr << "Invalid path: final element is not an object." << std::endl;
        return false;
    }

    auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                           { return kv.key == finalKey; });
    if (it != target->objectValue.end())
    {
        std::cerr << "Element already exists at path: " << path << std::endl;
        return false;
    }

    try
    {
        Lexer valueLexer(newValue);
        Parser valueParser(newValue);

        valueParser.currentToken = valueLexer.nextToken();

        JSONValue newParsedValue = valueParser.parseValue();
        target->objectValue.push_back(KeyValue(finalKey, new JSONValue(newParsedValue)));
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Invalid new value: " << e.what() << std::endl;
        return false;
    }
}

bool Parser::deleteElement(const std::string &path)
{
    std::vector<std::string> keys = splitPath(path);
    if (keys.empty())
    {
        std::cerr << "Invalid path." << std::endl;
        return false;
    }

    JSONValue *target = &root;
    for (size_t i = 0; i < keys.size() - 1; ++i)
    {
        if (target->type != JSONValueType::OBJECT)
        {
            std::cerr << "Invalid path: " << keys[i] << " is not an object." << std::endl;
            return false;
        }

        auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == keys[i]; });
        if (it == target->objectValue.end())
        {
            std::cerr << "Path element not found: " << keys[i] << std::endl;
            return false;
        }

        target = it->value;
    }

    std::string finalKey = keys.back();
    if (target->type != JSONValueType::OBJECT)
    {
        std::cerr << "Invalid path: final element is not an object." << std::endl;
        return false;
    }

    auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                           { return kv.key == finalKey; });
    if (it == target->objectValue.end())
    {
        std::cerr << "Element not found at path: " << path << std::endl;
        return false;
    }

    delete it->value;
    target->objectValue.erase(it);

    return true;
}

bool Parser::move(const std::string &from, const std::string &to)
{
    std::vector<std::string> fromKeys = splitPath(from);
    std::vector<std::string> toKeys = splitPath(to);

    if (fromKeys.empty() || toKeys.empty())
    {
        std::cerr << "Invalid path." << std::endl;
        return false;
    }

    JSONValue *fromTarget = &root;
    for (size_t i = 0; i < fromKeys.size() - 1; ++i)
    {
        if (fromTarget->type != JSONValueType::OBJECT)
        {
            std::cerr << "Invalid path: " << fromKeys[i] << " is not an object." << std::endl;
            return false;
        }

        auto it = std::find_if(fromTarget->objectValue.begin(), fromTarget->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == fromKeys[i]; });
        if (it == fromTarget->objectValue.end())
        {
            std::cerr << "Path element not found: " << fromKeys[i] << std::endl;
            return false;
        }

        fromTarget = it->value;
    }

    std::string finalFromKey = fromKeys.back();
    if (fromTarget->type != JSONValueType::OBJECT)
    {
        std::cerr << "Invalid path: final element is not an object." << std::endl;
        return false;
    }

    auto fromIt = std::find_if(fromTarget->objectValue.begin(), fromTarget->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == finalFromKey; });
    if (fromIt == fromTarget->objectValue.end())
    {
        std::cerr << "Element not found at path: " << from << std::endl;
        return false;
    }

    JSONValue *fromValue = fromIt->value;

    JSONValue *toTarget = &root;
    for (size_t i = 0; i < toKeys.size() - 1; ++i)
    {
        if (toTarget->type != JSONValueType::OBJECT)
        {
            std::cerr << "Invalid path: " << toKeys[i] << " is not an object." << std::endl;
            return false;
        }

        auto it = std::find_if(toTarget->objectValue.begin(), toTarget->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == toKeys[i]; });
        if (it == toTarget->objectValue.end())
        {
            JSONValue *newObject = new JSONValue();
            newObject->type = JSONValueType::OBJECT;
            toTarget->objectValue.push_back(KeyValue(toKeys[i], newObject));
            toTarget = newObject;
        }
        else
        {
            toTarget = it->value;
        }
    }

    std::string finalToKey = toKeys.back();
    if (toTarget->type != JSONValueType::OBJECT)
    {
        std::cerr << "Invalid path: final element is not an object." << std::endl;
        return false;
    }

    auto toIt = std::find_if(toTarget->objectValue.begin(), toTarget->objectValue.end(), [&](const KeyValue &kv)
                             { return kv.key == finalToKey; });
    if (toIt != toTarget->objectValue.end())
    {
        std::cerr << "Element already exists at path: " << to << std::endl;
        return false;
    }

    toTarget->objectValue.push_back(KeyValue(finalToKey, fromValue));
    fromTarget->objectValue.erase(fromIt);

    return true;
}

bool Parser::save(const std::string &path)
{
    JSONValue *value = path.empty() ? &root : findValueByPath(path);
    if (value == nullptr)
    {
        std::cerr << "Invalid path." << std::endl;
        return false;
    }

    try
    {
        std::string savePath = path.empty() ? currentFilePath : path;
        writeJSONToFile(*value, savePath);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving to file: " << e.what() << std::endl;
        return false;
    }
}

bool Parser::saveas(const std::string &file, const std::string &path)
{
    JSONValue *value = path.empty() ? &root : findValueByPath(path);
    if (value == nullptr)
    {
        std::cerr << "Invalid path." << std::endl;
        return false;
    }

    try
    {
        writeJSONToFile(*value, file);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error saving to file: " << e.what() << std::endl;
        return false;
    }
}

void Parser::writeToFile(const std::string &filePath)
{
    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Could not open file to write.");
    }

    writeJSON(outFile, root, 0);
    outFile.close();
}

void Parser::writeJSONToFile(const JSONValue &value, const std::string &filePath)
{
    std::ofstream outFile(filePath);
    if (!outFile.is_open())
    {
        throw std::runtime_error("Could not open file to write.");
    }

    writeJSON(outFile, value, 0);
    outFile.close();
}

void Parser::writeJSON(std::ostream &out, const JSONValue &value, int indent = 0) const
{
    std::string indentStr(indent, ' ');
    switch (value.type)
    {
    case JSONValueType::OBJECT:
        out << "{\n";
        for (size_t i = 0; i < value.objectValue.size(); ++i)
        {
            out << indentStr << "  \"" << value.objectValue[i].key << "\": ";
            writeJSON(out, *value.objectValue[i].value, indent + 2);
            if (i < value.objectValue.size() - 1)
                out << ",";
            out << "\n";
        }
        out << indentStr << "}";
        break;
    case JSONValueType::ARRAY:
        out << "[\n";
        for (size_t i = 0; i < value.arrayValue.size(); ++i)
        {
            out << indentStr << "  ";
            writeJSON(out, *value.arrayValue[i], indent + 2);
            if (i < value.arrayValue.size() - 1)
                out << ",";
            out << "\n";
        }
        out << indentStr << "]";
        break;
    case JSONValueType::STRING:
        out << "\"" << value.stringValue << "\"";
        break;
    case JSONValueType::NUMBER:
        out << value.numberValue;
        break;
    case JSONValueType::BOOL:
        out << (value.boolValue ? "true" : "false");
        break;
    case JSONValueType::NIL:
        out << "null";
        break;
    default:
        throw std::runtime_error("Unknown JSONType encountered in writeJSON.");
    }
}

JSONValue *Parser::findValueByPath(const std::string &path)
{
    if (path.empty())
    {
        return &root;
    }

    std::vector<std::string> keys = splitPath(path);
    JSONValue *target = &root;

    for (const auto &key : keys)
    {
        if (target->type != JSONValueType::OBJECT)
        {
            std::cerr << "Invalid path: " << key << " is not an object." << std::endl;
            return nullptr;
        }

        auto it = std::find_if(target->objectValue.begin(), target->objectValue.end(), [&](const KeyValue &kv)
                               { return kv.key == key; });
        if (it == target->objectValue.end())
        {
            std::cerr << "Path element not found: " << key << std::endl;
            return nullptr;
        }

        target = it->value;
    }

    return target;
}

JSONValue Parser::parseValue()
{
    switch (currentToken.type)
    {
    case TokenType::LEFT_BRACE:
        return parseObject();
    case TokenType::LEFT_BRACKET:
        return parseArray();
    case TokenType::STRING:
        return parseString();
    case TokenType::NUMBER:
        return parseNumber();
    case TokenType::TRUE:
        currentToken = lexer.nextToken();
        return parseBool(true);
    case TokenType::FALSE:
        currentToken = lexer.nextToken();
        return parseBool(false);
    case TokenType::NULL_TYPE:
        currentToken = lexer.nextToken();
        return parseNull();
    default:
        throw std::runtime_error("Unexpected token at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
    }
}

JSONValue Parser::parseObject()
{
    JSONValue objectValue;
    objectValue.type = JSONValueType::OBJECT;

    currentToken = lexer.nextToken();
    if (currentToken.type != TokenType::RIGHT_BRACE)
    {
        while (true)
        {
            if (currentToken.type != TokenType::STRING)
            {
                throw std::runtime_error("Expected string key");
            }
            std::string key = currentToken.value;
            currentToken = lexer.nextToken();

            if (currentToken.type != TokenType::COLON)
            {
                throw std::runtime_error("Expected ':'");
            }
            currentToken = lexer.nextToken();
            objectValue.objectValue.push_back(KeyValue(key, new JSONValue(parseValue())));
            if (currentToken.type == TokenType::COMMA)
            {
                currentToken = lexer.nextToken();
            }
            else
            {
                break;
            }
        }
        if (currentToken.type != TokenType::RIGHT_BRACE)
            throw std::runtime_error("Expected '}' at line " + std::to_string(lexer.getLine()) + ", column: " + std::to_string(lexer.getColumn()));
    }

    currentToken = lexer.nextToken();
    return objectValue;
}

JSONValue Parser::parseArray()
{
    JSONValue arrayValue;
    arrayValue.type = JSONValueType::ARRAY;

    currentToken = lexer.nextToken();
    if (currentToken.type != TokenType::RIGHT_BRACKET)
    {
        while (true)
        {
            arrayValue.arrayValue.push_back(new JSONValue(parseValue()));
            if (currentToken.type == TokenType::COMMA)
            {
                currentToken = lexer.nextToken();
            }
            else
            {
                break;
            }
        }
        if (currentToken.type != TokenType::RIGHT_BRACKET)
            throw std::runtime_error("Expected ']' at line " + std::to_string(lexer.getLine()) + ", column: " + std::to_string(lexer.getColumn()));
    }

    currentToken = lexer.nextToken();
    return arrayValue;
}

JSONValue Parser::parseString()
{
    JSONValue stringValue;
    stringValue.type = JSONValueType::STRING;
    stringValue.stringValue = currentToken.value;
    currentToken = lexer.nextToken();
    return stringValue;
}

JSONValue Parser::parseNumber()
{
    JSONValue numberValue;
    numberValue.type = JSONValueType::NUMBER;
    numberValue.numberValue = std::stod(currentToken.value);
    currentToken = lexer.nextToken();
    return numberValue;
}

JSONValue Parser::parseBool(bool value)
{
    JSONValue boolValue;
    boolValue.type = JSONValueType::BOOL;
    boolValue.boolValue = value;
    currentToken = lexer.nextToken();
    return boolValue;
}

JSONValue Parser::parseNull()
{
    JSONValue nullValue;
    nullValue.type = JSONValueType::NIL;
    currentToken = lexer.nextToken();
    return nullValue;
}

bool Parser::containsHelper(const JSONValue &jsonValue, const std::string &value) const
{
    switch (jsonValue.type)
    {
    case JSONValueType::OBJECT:
        for (const auto &kv : jsonValue.objectValue)
        {
            if (containsHelper(*kv.value, value))
            {
                return true;
            }
        }
        break;
    case JSONValueType::ARRAY:
        for (const auto &val : jsonValue.arrayValue)
        {
            if (containsHelper(*val, value))
            {
                return true;
            }
        }
        break;
    case JSONValueType::STRING:
        if (jsonValue.stringValue.find(value) != std::string::npos)
        {
            return true;
        }
        break;
    case JSONValueType::BOOL:
    case JSONValueType::NUMBER:
    case JSONValueType::NIL:
        break;
    default:
        throw std::runtime_error("Invalid JSON type encountered in containsHelper.");
    }

    return false;
}

std::vector<std::string> Parser::splitPath(const std::string &path) const
{
    std::vector<std::string> keys;
    size_t start = 0;
    size_t end = path.find('/');

    while (end != std::string::npos)
    {
        keys.push_back(path.substr(start, end - start));
        start = end + 1;
        end = path.find('/', start);
    }

    keys.push_back(path.substr(start));
    return keys;
}

void Parser::validateValue()
{
    switch (currentToken.type)
    {
    case TokenType::LEFT_BRACE:
        validateObject();
        break;
    case TokenType::LEFT_BRACKET:
        validateArray();
        break;
    case TokenType::STRING:
        validateString();
        break;
    case TokenType::NUMBER:
        validateNumber();
        break;
    case TokenType::TRUE:
    case TokenType::FALSE:
    case TokenType::NULL_TYPE:
        currentToken = lexer.nextToken();
        break;
    default:
        throw std::runtime_error("Unexpected token at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
    }
}

void Parser::validateObject()
{
    currentToken = lexer.nextToken();
    if (currentToken.type != TokenType::RIGHT_BRACE)
    {
        while (true)
        {
            if (currentToken.type != TokenType::STRING)
                throw std::runtime_error("Expected string key at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
            validateString();
            if (currentToken.type != TokenType::COLON)
                throw std::runtime_error("Expected ':' at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
            currentToken = lexer.nextToken();
            validateValue();
            if (currentToken.type == TokenType::COMMA)
            {
                currentToken = lexer.nextToken();
            }
            else
            {
                break;
            }
        }
        if (currentToken.type != TokenType::RIGHT_BRACE)
            throw std::runtime_error("Expected '}' at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
    }
    currentToken = lexer.nextToken();
}

void Parser::validateArray()
{
    currentToken = lexer.nextToken();
    if (currentToken.type != TokenType::RIGHT_BRACKET)
    {
        while (true)
        {
            validateValue();
            if (currentToken.type == TokenType::COMMA)
            {
                currentToken = lexer.nextToken();
            }
            else
            {
                break;
            }
        }
        if (currentToken.type != TokenType::RIGHT_BRACKET)
            throw std::runtime_error("Expected ']' at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
    }
    currentToken = lexer.nextToken();
}

void Parser::validateString()
{
    if (currentToken.type != TokenType::STRING)
        throw std::runtime_error("Expected string at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
    currentToken = lexer.nextToken();
}

void Parser::validateNumber()
{
    if (currentToken.type != TokenType::NUMBER)
        throw std::runtime_error("Expected number at line " + std::to_string(lexer.getLine()) + ", column " + std::to_string(lexer.getColumn()));
    currentToken = lexer.nextToken();
}

void Parser::printJSON(const JSONValue &value, int indent = 0)
{
    std::string indentStr(indent, ' ');
    switch (value.type)
    {
    case JSONValueType::OBJECT:
    {
        std::cout << "{\n";
        for (size_t i = 0; i < value.objectValue.size(); ++i)
        {
            std::cout << indentStr << "  \"" << value.objectValue[i].key << "\": ";
            printJSON(*value.objectValue[i].value, indent + 2);
            if (i < value.objectValue.size() - 1)
                std::cout << ",";
            std::cout << "\n";
        }
        std::cout << indentStr << "}";
        break;
    }
    case JSONValueType::ARRAY:
    {
        std::cout << "[\n";
        for (size_t i = 0; i < value.arrayValue.size(); ++i)
        {
            std::cout << indentStr << "  ";
            printJSON(*value.arrayValue[i], indent + 2);
            if (i < value.arrayValue.size() - 1)
                std::cout << ",";
            std::cout << "\n";
        }
        std::cout << indentStr << "]";
        break;
    }
    case JSONValueType::STRING:
        std::cout << "\"" << value.stringValue << "\"";
        break;
    case JSONValueType::NUMBER:
        std::cout << value.numberValue;
        break;
    case JSONValueType::BOOL:
        std::cout << (value.boolValue ? "true" : "false");
        break;
    case JSONValueType::NIL:
        std::cout << "null";
        break;
    }
}

void Parser::printOperation(const JSONValue &json)
{
    printJSON(json);
    std::cout << std::endl;
}