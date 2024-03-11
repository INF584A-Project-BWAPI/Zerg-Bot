#include "json.hpp"
#include "GameFileParser.hpp"
#include <iostream>
#include <filesystem>  // For C++17 and later, use #include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;

GameFileParser::GameFileParser() {
    // Building mapping
    unit_type_map["pylon"] = BWAPI::UnitTypes::Protoss_Pylon;
    unit_type_map["nexus"] = BWAPI::UnitTypes::Protoss_Nexus;
    unit_type_map["gateway"] = BWAPI::UnitTypes::Protoss_Gateway;
    unit_type_map["assimilator"] = BWAPI::UnitTypes::Protoss_Assimilator;
    unit_type_map["forge"] = BWAPI::UnitTypes::Protoss_Forge;
    unit_type_map["cybernetics_core"] = BWAPI::UnitTypes::Protoss_Cybernetics_Core;
    unit_type_map["shield_battery"] = BWAPI::UnitTypes::Protoss_Shield_Battery;
    unit_type_map["photon_cannon"] = BWAPI::UnitTypes::Protoss_Photon_Cannon;
    unit_type_map["robotics_facility"] = BWAPI::UnitTypes::Protoss_Robotics_Facility;
    unit_type_map["robotics_support_bay"] = BWAPI::UnitTypes::Protoss_Robotics_Support_Bay;
    unit_type_map["observatory"] = BWAPI::UnitTypes::Protoss_Observatory;
    unit_type_map["stargate"] = BWAPI::UnitTypes::Protoss_Stargate;
    unit_type_map["fleet_beacon"] = BWAPI::UnitTypes::Protoss_Fleet_Beacon;
    unit_type_map["arbiter_tribunal"] = BWAPI::UnitTypes::Protoss_Arbiter_Tribunal;
    unit_type_map["citadel_of_adun"] = BWAPI::UnitTypes::Protoss_Citadel_of_Adun;
    unit_type_map["templar_archives"] = BWAPI::UnitTypes::Protoss_Templar_Archives;

    // Unit mapping
    unit_type_map["probe"] = BWAPI::UnitTypes::Protoss_Probe;
    unit_type_map["zealot"] = BWAPI::UnitTypes::Protoss_Zealot;

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
        BuildType buildType = parse_buildtype_enum(building["buildType"]);
        ProducerType producerType = parse_producertype_enum(building["producerType"]);

        buildings.emplace_back(name, buildType, producerType);
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
