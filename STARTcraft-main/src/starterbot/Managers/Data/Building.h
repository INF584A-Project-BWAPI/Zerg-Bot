#pragma once

#include <vector>;
#include "../../BT/Data.h"
//#include "Data.h"

enum class BuildingStatus {
	UnderConstruction,
	OrderGiven,
	Constructed,
	UnAssigned
};


class Building {

public:
	// Fields
	BWAPI::TilePosition position;
	BWAPI::UnitType unitType;
	BWAPI::Unit unit; // The actual unit instance, and not the type
	BuildingStatus status;

	// Functions
	Building(BWAPI::TilePosition p, BWAPI::UnitType t);

	bool operator == (const Building& b);
};

