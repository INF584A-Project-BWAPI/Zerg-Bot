#include <iostream>
#include <fstream>
#include "json.hpp"
#include <vector>
#include <string>
#include "Tools.h"
#include "Data.h"
#include <BWAPI/Type.h>

using namespace std;


#ifndef GAME_FILE_PARSER_H
#define GAME_FILE_PARSER_H

/*
== DATACLASSES USED IN PARSING
==============================
*/

class BuildingRecipe {
public:
    // Constructor
    BuildingRecipe(const BWAPI::UnitType& name, int minProductionLevel, int maxProductionLevel)
        : name(name), minProductionLevel(minProductionLevel), maxProductionLevel(maxProductionLevel) {}

    // Accessors
    BWAPI::UnitType getName() const { return name; }
    int getMinProductionLevel() const { return minProductionLevel; }
    int getMaxProductionLevel() const { return maxProductionLevel; }

private:
    BWAPI::UnitType name;
    int minProductionLevel;
    int maxProductionLevel;
};


/*
== Parses the JSON file into appropriate datatypes
==================================================
*/

class GameFileParser
{
public:
    std::vector<BuildingRecipe> buildorder;
    nlohmann::json json_file;

    GameFileParser();

    int parse_game_file(string const path);

    // Debugging
    void print_build_order();

private:
    bool json_loaded;
    std::unordered_map<std::string, BWAPI::UnitType> unit_type_map;


    int parse_build_order();
};

#endif // GAME_FILE_PARSER_H