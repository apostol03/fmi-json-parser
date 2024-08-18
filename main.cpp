#include <fstream>
#include <iostream>

#include "Parser.h"
#include "Engine.h"

int main()
{
    // Sample commands for execution [open example.json]
    // create newPath "newValue"
    // set newPath 20
    // delete newPath
    // move management newPath
    // saveas a.json newPath

    try
    {
        Engine engine;
        engine.prompt();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}