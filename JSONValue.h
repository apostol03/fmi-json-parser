#ifndef JSON_VALUE_H
#define JSON_VALUE_H

#include <vector>
#include <regex>
#include <cmath>

/**
 * Enum representing the type of a JSON value.
 */
enum class JSONValueType
{
    OBJECT,
    ARRAY,
    STRING,
    NUMBER,
    BOOL,
    NIL
};

/**
 * Class representing a JSON value.
 */
class JSONValue;

/**
 * Structure representing a key-value pair in a JSON object.
 */
struct KeyValue
{
    std::string key;
    JSONValue *value;

    /**
     * Constructs a KeyValue object with the given key and value.
     * @param k Key of the JSON object.
     * @param v Pointer to the JSON value associated with the key.
     */
    KeyValue(const std::string &k, JSONValue *v) : key(k), value(v) {}
};

/**
 * Class representing a JSON value.
 */
class JSONValue
{
public:
    JSONValueType type;
    std::string stringValue;
    double numberValue;
    bool boolValue;
    std::vector<JSONValue *> arrayValue;
    std::vector<KeyValue> objectValue;

public:
    JSONValue();

    JSONValue(const JSONValue &other);

    JSONValue &operator=(const JSONValue &other);

    ~JSONValue();

    /**
     * Converts the JSON value to a string representation.
     * @return String representation of the JSON value.
     */
    std::string toString() const;

    /**
     * Searches for a key in the JSON value and collects all matching values.
     * @param key Key to search for.
     * @param results Vector to store pointers to matching JSON values.
     */
    void searchKey(const std::string &key, std::vector<JSONValue *> &results) const;

    /**
     * Searches for keys matching a regex pattern in the JSON value and collects all matching values.
     * @param pattern Regex pattern to match keys against.
     * @param results Vector to store pointers to matching JSON values.
     */
    void searchKey(const std::regex &pattern, std::vector<JSONValue *> &results) const;

private:
    /**
     * Copies the contents of another JSONValue.
     * @param other JSONValue to copy from.
     */
    void copy(const JSONValue &other);

    /**
     * Clears the contents of the JSONValue.
     */
    void clear();
};

#endif