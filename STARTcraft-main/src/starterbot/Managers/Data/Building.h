#pragma once

#include <vector>;
#include "Data.h"

enum class BuildingStatus {
	UnderConstruction,
	OrderGiven, // A worker is on the way to build this building (resources not used yet)
	Constructed,
	UnAssigned
};


class Building {

public:
	// Fields
	BWAPI::TilePosition position;
	BWAPI::UnitType unitType;
	BWAPI::Unit unit; // The actual unit instance, and not the type
	BuildingStatus status; // Used to check when a building is done constructing and we can finally produce units with it.

	// Functions
	Building(BWAPI::TilePosition p, BWAPI::UnitType t);

	bool operator == (const Building& b); // Compare building based on position and its UnitType
};

