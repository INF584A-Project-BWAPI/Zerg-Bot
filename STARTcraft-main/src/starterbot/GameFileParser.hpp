#include <iostream>
#include <fstream>
#include "json.hpp"
#include <vector>
#include <string>

using namespace std;


#ifndef GAME_FILE_PARSER_H
#define GAME_FILE_PARSER_H

/*
=================================
== DATACLASSES USED IN PARSING ==
=================================
*/

class BuildingRecipe {
public:
    // Constructor
    BuildingRecipe(const std::string& name, int minProductionLevel, int maxProductionLevel)
        : name(name), minProductionLevel(minProductionLevel), maxProductionLevel(maxProductionLevel) {}

    // Accessors
    std::string getName() const { return name; }
    int getMinProductionLevel() const { return minProductionLevel; }
    int getMaxProductionLevel() const { return maxProductionLevel; }

private:
    std::string name;
    int minProductionLevel;
    int maxProductionLevel;
};




class GameFileParser
{
public:
    std::vector<BuildingRecipe> buildorder;
    nlohmann::json json_file;

    int parse_game_file(string const path);

    // Debugging
    void print_build_order();

private:
    bool json_loaded;

    int parse_build_order();

};

#endif // HEADER_NAME_H