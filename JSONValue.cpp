#include "JSONValue.h"

JSONValue::JSONValue() : type(JSONValueType::NIL), numberValue(0), boolValue(false) {}

JSONValue::JSONValue(const JSONValue &other)
{
    copy(other);
}

JSONValue &JSONValue::operator=(const JSONValue &other)
{
    if (this != &other)
    {
        this->~JSONValue();
        copy(other);
    }

    return *this;
}

JSONValue::~JSONValue()
{
    for (auto val : arrayValue)
        delete val;
    for (auto &kv : objectValue)
        delete kv.value;
}

std::string JSONValue::toString() const
{
    switch (type)
    {
    case JSONValueType::STRING:
        return "\"" + stringValue + "\"";
    case JSONValueType::NUMBER:
        if (numberValue == std::floor(numberValue))
        {
            return std::to_string(static_cast<int>(numberValue));
        }
        else
        {
            return std::to_string(numberValue);
        }
    case JSONValueType::BOOL:
        return boolValue ? "true" : "false";
    case JSONValueType::ARRAY:
    {
        std::string result;
        for (size_t i = 0; i < arrayValue.size(); i++)
        {
            if (i > 0)
                result += ", \n";
            result += arrayValue[i]->toString();
        }
        return result;
    }
    case JSONValueType::OBJECT:
    {
        std::string result = "  {\n";
        for (size_t i = 0; i < objectValue.size(); i++)
        {
            if (i > 0)
                result += ", \n";
            result += "\t\"" + objectValue[i].key + "\": " + objectValue[i].value->toString();
        }
        result += "\n  }";
        return result;
    }
    case JSONValueType::NIL:
        return "null";
    default:
        return "";
    }
}

void JSONValue::searchKey(const std::string &key, std::vector<JSONValue *> &results) const
{
    searchKey(std::regex(key), results);
}

void JSONValue::searchKey(const std::regex &pattern, std::vector<JSONValue *> &results) const
{
    switch (type)
    {
    case JSONValueType::OBJECT:
        for (const auto &kv : objectValue)
        {
            if (std::regex_match(kv.key, pattern))
            {
                results.push_back(kv.value);
            }
            kv.value->searchKey(pattern, results);
        }
        break;
    case JSONValueType::ARRAY:
        for (const auto &item : arrayValue)
        {
            item->searchKey(pattern, results);
        }
        break;
    default:
        break;
    }
}

void JSONValue::copy(const JSONValue &other)
{
    type = other.type;
    stringValue = other.stringValue;
    numberValue = other.numberValue;
    boolValue = other.boolValue;

    for (const auto &val : other.arrayValue)
    {
        arrayValue.push_back(new JSONValue(*val));
    }

    for (const auto &kv : other.objectValue)
    {
        objectValue.push_back(KeyValue(kv.key, new JSONValue(*kv.value)));
    }
}

void JSONValue::clear()
{
    for (auto val : arrayValue)
        delete val;
    for (auto &kv : objectValue)
        delete kv.value;

    arrayValue.clear();
    objectValue.clear();

    type = JSONValueType::NIL;
    stringValue.clear();
    numberValue = 0;
    boolValue = false;
}