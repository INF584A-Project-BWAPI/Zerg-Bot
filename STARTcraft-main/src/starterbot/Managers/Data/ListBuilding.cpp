#include "ListBuilding.h"

ListBuilding::ListBuilding() {}

void ListBuilding::removeBuilding(const Building& b)
{
    const auto& building = std::find(buildings.begin(), buildings.end(), b);
    if (building != buildings.end()){ buildings.erase(building); }
}

std::vector<Building>& ListBuilding::getBuildings()
{
    return buildings;
}

void ListBuilding::addBuilding(const Building& b)
{
    buildings.push_back(b);
}

bool ListBuilding::isBeingBuilt(BWAPI::UnitType type)
{
    for (auto& b : buildings) {
        if (b.unitType == type) {
            return true;
        }
    }

    return false;
}

void ListBuilding::removeBuildings(const std::vector<Building>& buildings)
{
    for (const auto& b : buildings) {
        removeBuilding(b);
    }
}