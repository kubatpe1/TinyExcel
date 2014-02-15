#include <string>
#include <vector>
#include <iostream>

#include "interactive.h"
#include "exceptions.h"
#include "parsing.h"

void InteractiveContext::load(const std::string &fname)
{
    try
    {
        t.load(fname);
    }
    catch (FileOpenException &ex)
    {
        std::cerr << "File " << fname << " can't be opened!" << std::endl;
    }
}

void InteractiveContext::save(const std::string &fname)
{
    try
    {
        t.save(fname, true);
    }
    catch (FileOpenException &ex)
    {
        std::cerr << "File " << fname << " can't be opened!" << std::endl;
    }
}

void InteractiveContext::saveval(const std::string &fname)
{
    try
    {
        t.save(fname, false);
    }
    catch (FileOpenException &ex)
    {
        std::cerr << "File " << fname << " can't be opened!" << std::endl;
    }
}


void InteractiveContext::print()
{
    t.print(std::cout);
}

void InteractiveContext::help()
{
    std::cout << "TinyExcel Interactive Mode" << std::endl;
    std::cout << "commands:" << std::endl;
    std::cout << "  load fname ... Loads table" << std::endl;
    std::cout << "  save fname ... Saves table" << std::endl;
    std::cout << "  saveval fname ... Saves values from table" << std::endl;
    std::cout << "  eval ... Evaluates the table" << std::endl;
    std::cout << "  print ... Prints the table content" << std::endl;
    std::cout << "  get cell ... Evaluates cell and prints its value" << std::endl;
    std::cout << "  set cell content ... Sets cell content (resets previously computed values)" << std::endl;
    std::cout << "  help ... Shows this help" << std::endl;
}

void InteractiveContext::eval()
{
    try
    {
        t.evaluate();
    }
    catch (EvaluationException &ex)
    {
        std::cerr << "Error while evaluating the table!" << std::endl;
    }
}

void InteractiveContext::getc(const std::string &cell)
{
    try
    {
        CellReference r = coords_to_reference(cell);
        t.evaluate_cell(r);
        Cell &c = t.get_cell(r);
        std::cout << c.get_content() << std::endl;
    }
    catch (InvalidCoordinatesException &ex)
    {
        std::cerr << "Invalid cell coordinates: " << cell << std::endl;
        return;
    }
    catch (EvaluationException &ex)
    {
        std::cerr << "Error while evaluating the cell!" << std::endl;
    }
}

void InteractiveContext::setc(const std::string &cell, const std::string &content)
{
    try
    {
        CellReference r = coords_to_reference(cell);
        t.set_cell(r, content);
        t.reset();
    }
    catch (InvalidCoordinatesException &ex)
    {
        std::cerr << "Invalid cell coordinates: " << cell << std::endl;
        return;
    }
}

bool InteractiveContext::execute_command(const std::string &cmd)
{
    const std::vector<char> delims = {' '};
    std::vector<std::string> parts;
    parts = split_string(cmd, delims, true, false);

    if (parts.size() == 1 && parts[0] == "") return false;

    if (parts[0] == "load")
    {
        if (parts.size() < 3)
        {
            throw NotEnoughArgumentsException();
        }
        load(parts[2]);
        return false;
    }
    if (parts[0] == "save")
    {
        if (parts.size() < 3)
        {
            throw NotEnoughArgumentsException();
        }
        save(parts[2]);
        return false;
    }
    if (parts[0] == "saveval")
    {
        if (parts.size() < 3)
        {
            throw NotEnoughArgumentsException();
        }
        saveval(parts[2]);
        return false;
    }
    if (parts[0] == "get")
    {
        if (parts.size() < 3)
        {
            throw NotEnoughArgumentsException();
        }
        getc(parts[2]);
        return false;
    }
    if (parts[0] == "set")
    {
        if (parts.size() < 5)
        {
            throw NotEnoughArgumentsException();
        }
        std::string coords = parts[2];
        parts.erase(parts.begin(), parts.begin() + 4);
        std::string content = merge_string(parts, "");
        trim(content, ' ');
        setc(coords, content);
        return false;
    }
    else if (parts[0] == "print")
    {
        print();
        return false;
    }
    else if (parts[0] == "eval")
    {
        eval();
        return false;
    }
    else if (parts[0] == "help")
    {
        help();
        return false;
    }
    else if (parts[0] == "exit")
    {
        return true;
    }
    else
    {
        throw InvalidCommandException();
    }

    return false;
}

// Performs the interactive loop
// - reads user commands, parses it and executes it
void InteractiveContext::start_loop(std::istream &in)
{
    std::string line;
    while (!in.eof())
    {
        std::cout << "TinyExcel> ";
        getline(in, line);
        try
        {
            if (execute_command(line)) break;
        }
        catch (InvalidCommandException &ex)
        {
            std::cerr << "Invalid command: " << line << std::endl;
        }
        catch (NotEnoughArgumentsException &ex)
        {
            std::cerr << "Not enough arguments for a command: " << line << std::endl;
        }
    }
}
