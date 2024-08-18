#ifndef PARSER_H
#define PARSER_H

#include <fstream>

#include "Lexer.h"
#include "JSONValue.h"

/**
 * Class responsible for parsing, manipulating, and validating JSON data.
 */
class Parser
{
    Lexer lexer;
    Token currentToken;
    JSONValue root;
    std::string currentFilePath;

public:
    /**
     * Constructs a Parser object with the given JSON input.
     * @param input JSON input as a string.
     */
    Parser(const std::string &input, const std::string &currentFilePath);

    /**
     * Parses the JSON input and returns the root JSONValue.
     * @return Root JSONValue of the parsed JSON structure.
     */
    JSONValue parse();

    /**
     * Validates the JSON structure.
     * @return True if the JSON structure is valid, false otherwise.
     */
    bool validate();

    /**
     * Searches for a key in the JSON structure.
     * @param key Key to search for.
     * @return Vector of pointers to JSONValue that match the key.
     */
    std::vector<JSONValue *> searchKey(const std::string &key) const;

    /**
     * Searches for keys matching a regex pattern in the JSON structure.
     * @param pattern Regex pattern to match keys against.
     * @return Vector of pointers to JSONValue that match the pattern.
     */
    std::vector<JSONValue *> searchKey(const std::regex &pattern) const;

    /**
     * Checks if a value is contained in the JSON structure.
     * @param value Value to search for.
     * @return True if the value is found, false otherwise.
     */
    bool contains(const std::string &value) const;

    /**
     * Sets a new value at the specified path in the JSON structure.
     * @param path Path to the element to be updated.
     * @param newValue New value to set.
     * @return True if the value is successfully set, false otherwise.
     */
    bool set(const std::string &path, const std::string &newValue);

    /**
     * Creates a new element at the specified path in the JSON structure.
     * @param path Path to the new element to be created.
     * @param newValue Value of the new element.
     * @return True if the element is successfully created, false otherwise.
     */
    bool create(const std::string &path, const std::string &newValue);

    /**
     * Deletes an element at the specified path in the JSON structure.
     * @param path Path to the element to be deleted.
     * @return True if the element is successfully deleted, false otherwise.
     */
    bool deleteElement(const std::string &path);

    /**
     * Moves elements from one path to another in the JSON structure.
     * @param from Source path.
     * @param to Destination path.
     * @return True if the elements are successfully moved, false otherwise.
     */
    bool move(const std::string &from, const std::string &to);

    /**
     * Saves the JSON structure to a file.
     * @param path Optional path within the JSON structure to save.
     * @return True if the JSON is successfully saved, false otherwise.
     */
    bool save(const std::string &path);

    /**
     * Saves the JSON structure to a specified file.
     * @param file Path to the file where the JSON will be saved.
     * @param path Optional path within the JSON structure to save.
     * @return True if the JSON is successfully saved, false otherwise.
     */
    bool saveas(const std::string &file, const std::string &path);

    /**
     * Writes the JSON structure to a file.
     * @param filePath Path to the file.
     */
    void writeToFile(const std::string &filePath);

    /**
     * Writes a JSONValue to a file.
     * @param value JSONValue to be written.
     * @param filePath Path to the file.
     */
    void writeJSONToFile(const JSONValue &value, const std::string &filePath);

private:
    /**
     * Writes a JSONValue to an output stream with indentation.
     * @param out Output stream.
     * @param value JSONValue to be written.
     * @param indent Current indentation level.
     */
    void writeJSON(std::ostream &out, const JSONValue &value, int indent) const;

    /**
     * Finds a JSONValue by a given path.
     * @param path Path to the JSON element.
     * @return Pointer to the JSONValue if found, nullptr otherwise.
     */
    JSONValue *findValueByPath(const std::string &path);

    /**
     * Parses a JSON value.
     * @return Parsed JSONValue.
     */
    JSONValue parseValue();

    /**
     * Parses a JSON object.
     * @return Parsed JSONValue representing the object.
     */
    JSONValue parseObject();

    /**
     * Parses a JSON array.
     * @return Parsed JSONValue representing the array.
     */
    JSONValue parseArray();

    /**
     * Parses a JSON string.
     * @return Parsed JSONValue representing the string.
     */
    JSONValue parseString();

    /**
     * Parses a JSON number.
     * @return Parsed JSONValue representing the number.
     */
    JSONValue parseNumber();

    /**
     * Parses a JSON boolean.
     * @param value Boolean value.
     * @return Parsed JSONValue representing the boolean.
     */
    JSONValue parseBool(bool value);
    
    /**
     * Parses a JSON null value.
     * @return Parsed JSONValue representing null.
     */
    JSONValue parseNull();

    /**
     * Helper function to check if a value is contained in a JSONValue.
     * @param jsonValue JSONValue to check.
     * @param value Value to search for.
     * @return True if the value is found, false otherwise.
     */
    bool containsHelper(const JSONValue &jsonValue, const std::string &value) const;

    /**
     * Splits a path string by '/' into individual keys.
     * @param path Path string.
     * @return Vector of keys.
     */
    std::vector<std::string> splitPath(const std::string &path) const;

private:
    /**
     * Validates a JSON value.
     */
    void validateValue();

    /**
     * Validates a JSON object.
     */
    void validateObject();

    /**
     * Validates a JSON array.
     */
    void validateArray();

    /**
     * Validates a JSON string.
     */
    void validateString();

    /**
     * Validates a JSON number.
     */
    void validateNumber();

    /**
     * Prints a JSONValue.
     * @param json JSONValue to be printed.
     */
    void printOperation(const JSONValue &json);

public:
    /**
     * Prints a JSONValue with indentation.
     * @param value JSONValue to be printed.
     * @param indent Current indentation level.
     */
    void printJSON(const JSONValue &value, int indent);
};

#endif