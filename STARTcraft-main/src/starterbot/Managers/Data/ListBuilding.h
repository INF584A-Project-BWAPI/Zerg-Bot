#pragma once

#include <vector>;
#include "Data.h"
#include "Building.h";

class ListBuilding {
	std::vector<Building> buildings;

public:
    ListBuilding();

    std::vector<Building>& getBuildings();

    void addBuilding(const Building& b);
    void removeBuilding(const Building& b);
    void removeBuildings(const std::vector<Building>& buildings);
    bool isBeingBuilt(BWAPI::UnitType type);
};

