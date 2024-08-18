#include "Engine.h"

Engine::Engine() {}

void Engine::prompt()
{
    if (!fileLoaded)
    {
        std::cout << "Please enter the path of the json file you wish to manipulate." << std::endl;
        std::string filePath;
        std::getline(std::cin, filePath);
        openFile(filePath);
    }

    std::cout << "Welcome! Pick any of the following commands to operate with the JSON Parser: " << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;
    std::cout << "open <path> | validate | print | search <key> | " << std::endl;
    std::cout << "contains <value> | set <path> <string> | create <path> <string> | " << std::endl;
    std::cout << "delete <path> | move <from> <to> | save [<path>] | saveas <file> [<path>]" << std::endl;
    std::cout << "------------------------------------------------------------------------------" << std::endl;

    std::string command;
    while (true)
    {
        std::cout << "[" << currentFilePath << "] " << "> ";
        std::getline(std::cin, command);
        if (command == "exit")
        {
            break;
        }
        executeCommand(command);
    }
}

void Engine::executeCommand(const std::string &command)
{
    if (command.rfind("open ", 0) == 0)
    {
        std::string filePath = command.substr(5);
        openFile(filePath);
    }
    else if (command == "validate")
    {
        std::cout << (parser->validate() ? "Valid JSON!" : "Invalid JSON!") << std::endl;
    }
    else if (command == "print")
    {
        parser->printJSON(parser->parse(), 0);
        std::cout << std::endl;
    }
    else if (command.rfind("search ", 0) == 0)
    {
        std::string key = command.substr(7);
        auto results = parser->searchKey(key);
        std::cout << "[" << std::endl;
        for (const auto &result : results)
        {
            std::cout << result->toString() << std::endl;
        }
        std::cout << "]" << std::endl;
    }
    else if (command.rfind("contains ", 0) == 0)
    {
        std::string value = command.substr(9);
        if (parser->contains(value))
        {
            std::cout << "The value \"" << value << "\" is present in the JSON document." << std::endl;
        }
        else
        {
            std::cout << "The value \"" << value << "\" is not present in the JSON document." << std::endl;
        }
    }
    else if (command.rfind("set ", 0) == 0)
    {
        size_t pos = command.find(" ", 4);

        if (pos == std::string::npos)
        {
            std::cerr << "Invalid command format." << std::endl;
            return;
        }

        std::string path = command.substr(4, pos - 4);
        std::string value = command.substr(pos + 1);

        if (parser->set(path, value))
        {
            std::cout << "Successfully updated the value at path: " << path << std::endl;
        }
        else
        {
            std::cout << "Failed to update the value at path: " << path << std::endl;
        }
        parser->writeToFile(currentFilePath);
    }
    else if (command.rfind("create ", 0) == 0)
    {
        size_t pos = command.find(" ", 7);

        if (pos == std::string::npos)
        {
            std::cerr << "Invalid command format." << std::endl;
            return;
        }

        std::string path = command.substr(7, pos - 7);
        std::string value = command.substr(pos + 1);
        if (parser->create(path, value))
        {
            std::cout << "Successfully created the value at path: " << path << std::endl;
        }
        else
        {
            std::cout << "Failed to create the value at path: " << path << std::endl;
        }
        parser->writeToFile(currentFilePath);
    }
    else if (command.rfind("delete ", 0) == 0)
    {
        std::string path = command.substr(7);
        if (parser->deleteElement(path))
        {
            std::cout << "Successfully deleted the value at path: " << path << std::endl;
        }
        else
        {
            std::cout << "Failed to delete the value at path: " << path << std::endl;
        }
        parser->writeToFile(currentFilePath);
    }
    else if (command.rfind("move ", 0) == 0)
    {
        size_t pos = command.find(" ", 5);
        if (pos == std::string::npos)
        {
            std::cerr << "Invalid command format." << std::endl;
            return;
        }
        std::string from = command.substr(5, pos - 5);
        std::string to = command.substr(pos + 1);
        if (parser->move(from, to))
        {
            std::cout << "Successfully moved the value from path: " << from << " to path: " << to << std::endl;
        }
        else
        {
            std::cout << "Failed to move the value from path: " << from << " to path: " << to << std::endl;
        }
        parser->writeToFile(currentFilePath);
    }
    else if (command == "save")
    {
        if (parser->save(""))
        {
            std::cout << "Successfully saved JSON file " << currentFilePath << std::endl;
        }
        else
        {
            std::cout << "Failed to save the JSON" << std::endl;
        }
    }
    else if (command.rfind("save ", 0) == 0)
    {
        std::string path = command.substr(5);
        if (parser->save(path))
        {
            std::cout << "Successfully saved " << path << " in JSON file " << currentFilePath << std::endl;
        }
        else
        {
            std::cout << "Failed to save JSON path " << path << std::endl;
        }
    }
    else if (command.rfind("saveas ", 0) == 0)
    {
        size_t pos = command.find(" ", 7);
        if (pos == std::string::npos)
        {
            std::cerr << "Invalid command format." << std::endl;
            return;
        }
        std::string file = command.substr(7, pos - 7);
        std::string path = command.substr(pos + 1);

        if (path.empty())
        {
            if (parser->saveas(file, ""))
            {
                std::cout << "Successfully saved JSON to " << file << std::endl;
            }
            else
            {
                std::cout << "Failed to save JSON to " << file << std::endl;
            }
        }
        else
        {
            if (parser->saveas(file, path))
            {
                std::cout << "Successfully saved the JSON at path: " << path << " to " << file << std::endl;
            }
            else
            {
                std::cout << "Failed to save the JSON at path: " << path << " to " << file << std::endl;
            }
        }
    }
    else
    {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}

void Engine::openFile(const std::string &filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        std::cerr << "File does not exist. Creating a new empty file..." << std::endl;
        std::ofstream newFile(filePath);
        newFile << "{}";
        newFile.close();
        file.open(filePath);
        if (!file.is_open())
        {
            std::cout << "Could not create the file!" << std::endl;
            return;
        }
    }

    std::string input((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    try
    {
        parser = new Parser(input, filePath);
        fileLoaded = true;
        currentFilePath = filePath;
        std::cout << "Successfully loaded file: " << filePath << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error loading file: " << e.what() << std::endl;
        fileLoaded = false;
        delete parser;
        parser = nullptr;
    }
}