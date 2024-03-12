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

enum class ProducerType {
    Base,
    Worker,
};

static const std::map<std::string, ProducerType> producerTypeStrToEnum = {
    {"Base", ProducerType::Base},
    {"Worker", ProducerType::Worker},
};

enum class BuildType {
    Unit,
    Building,
};

static const std::map<std::string, BuildType> buildTypeStrToEnum = {
    {"Unit", BuildType::Unit},
    {"Building", BuildType::Building},
};

class BuildingRecipe {
public:
    // Constructor
    BuildingRecipe(const BWAPI::UnitType& name, const BuildType& type, ProducerType producer)
        : name(name), type(type), producer(producer) {}

    // Accessors
    BWAPI::UnitType getName() const { return name; }
    BuildType getType() const { return type; }
    ProducerType getProducer() const { return producer; }

private:
    BWAPI::UnitType name;
    BuildType type;
    ProducerType producer;
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
    BuildType parse_buildtype_enum(const string& type);
    ProducerType parse_producertype_enum(const string& type);
};

#endif // GAME_FILE_PARSER_H