#include "json.hpp"
#include "GameFileParser.hpp"
#include <iostream>
#include <filesystem>  // For C++17 and later, use #include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;

GameFileParser::GameFileParser() {
    // Add conversion table entries for the unit types
    unit_type_map["pylon"] = BWAPI::UnitTypes::Protoss_Pylon;
    unit_type_map["probe"] = BWAPI::UnitTypes::Protoss_Probe;
    unit_type_map["gateway"] = BWAPI::UnitTypes::Protoss_Gateway;
    unit_type_map["assimilator"] = BWAPI::UnitTypes::Protoss_Assimilator;

    cout << unit_type_map.size() << endl;
};

int GameFileParser::parse_game_file(string const path)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return 1;
    }

    json j;
    file >> j;
    file.close();

    json_file = j;
    json_loaded = true;

    parse_build_order();

    return 1;
}

void GameFileParser::print_build_order()
{
    for (const auto& building : buildorder) {
        std::cout << "Unit Type: " << building.getName() << std::endl;
        std::cout << "Min Production Level: " << building.getMinProductionLevel() << std::endl;
        std::cout << "Max Production Level: " << building.getMaxProductionLevel() << std::endl;
        std::cout << std::endl;
    }
}

int GameFileParser::parse_build_order()
{
    if (!json_loaded) {
        std::cerr << "JSON game file was not pre-loaded." << std::endl;
        return 0;
    }

    std::vector<BuildingRecipe> buildings;
    for (const auto& building : json_file["build_order"]) {
        BWAPI::UnitType name = unit_type_map[building["name"]];
        int minProductionLevel = building["minProductionLevel"];
        int maxProductionLevel = building["maxProductionLevel"];
        BuildType buildType = parse_buildtype_enum(building["buildType"]);
        ProducerType producerType = parse_producertype_enum(building["producerType"]);

        buildings.emplace_back(name, buildType, producerType, minProductionLevel, maxProductionLevel);
    }

    buildorder = buildings;

    return 1;
}

BuildType GameFileParser::parse_buildtype_enum(const string& type)
{
    auto it = buildTypeStrToEnum.find(type);
    if (it != buildTypeStrToEnum.end()) {
        return it->second;
    }
    else {
        // Handle error case when string doesn't match any enum value
        throw std::invalid_argument("Invalid BuildType string: " + type);
    }
}

ProducerType GameFileParser::parse_producertype_enum(const string& type)
{
    auto it = producerTypeStrToEnum.find(type);
    if (it != producerTypeStrToEnum.end()) {
        return it->second;
    }
    else {
        // Handle error case when string doesn't match any enum value
        throw std::invalid_argument("Invalid ProducerType string: " + type);
    }
}
