#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>
#include <string>

#include "Parser.h"

/**
 * Class responsible for handling user input and executing commands to manipulate JSON data using the Parser.
 */
class Engine
{
public:
    /**
     * Constructs an Engine object.
     */
    Engine();

    /**
     * Prompts the user for commands and executes them.
     * If no file is loaded, it first prompts the user to enter the path of the JSON file to manipulate.
     */
    void prompt();

private:
    /**
     * Executes the given command.
     * @param command The command to execute.
     */
    void executeCommand(const std::string &command);

    /**
     * Opens the specified file and loads its content into the parser.
     * If the file does not exist, it creates a new file with empty content.
     * @param filePath Path to the file to open.
     */
    void openFile(const std::string &filePath);

private:
    Parser *parser = nullptr;
    bool fileLoaded = false;
    std::string currentFilePath;
};

#endif
