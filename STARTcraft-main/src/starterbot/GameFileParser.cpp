#include "json.hpp"
#include "GameFileParser.hpp"
#include <iostream>
#include <filesystem>  // For C++17 and later, use #include <filesystem>
namespace fs = std::filesystem;

using json = nlohmann::json;

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
    fs::path currentPath = fs::current_path();
    std::cout << "Current absolute path: " << currentPath << std::endl;

    for (const auto& building : buildorder) {
        std::cout << "Name: " << building.getName() << std::endl;
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
        std::string name = building["name"];
        int minProductionLevel = building["minProductionLevel"];
        int maxProductionLevel = building["maxProductionLevel"];
        buildings.emplace_back(name, minProductionLevel, maxProductionLevel);
    }

    buildorder = buildings;

    return 1;
}
