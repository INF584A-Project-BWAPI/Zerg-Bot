#include "Building.h"


Building::Building(BWAPI::TilePosition p, BWAPI::UnitType t)
    : position(p)
    , unitType(t)
    , unit()
    , status(BuildingStatus::UnAssigned)
{}

bool Building::operator == (const Building & b)
{
    return (b.unit == unit)
        && (b.position.x == position.x)
        && (b.position.y == position.y);
}

