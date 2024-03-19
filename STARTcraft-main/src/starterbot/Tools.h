#pragma once

#include <BWAPI.h>

namespace Tools
{
    BWAPI::Unit GetClosestUnitTo(BWAPI::Position p, const BWAPI::Unitset& units);
    BWAPI::Unit GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units);

    int CountUnitsOfType(BWAPI::UnitType type, const BWAPI::Unitset& units);

    BWAPI::Unit GetUnitOfType(BWAPI::UnitType type);
    BWAPI::Unit GetDepot();

    bool BuildBuilding(BWAPI::UnitType type);
    std::vector<BWAPI::Unit> GetThreeClosestWorkersToTilePosition(const BWAPI::TilePosition& tilePos);
    void DrawUnitBoundingBoxes();
    void DrawUnitCommands();

    void SmartRightClick(BWAPI::Unit unit, BWAPI::Unit target);

    int GetUnusedSupply(bool inProgress = false);
    int GetTotalSupply(bool inProgress = false);

    void DrawUnitHealthBars();
    void DrawHealthBar(BWAPI::Unit unit, double ratio, BWAPI::Color color, int yOffset);
    BWAPI::Unit GetNexusAtTilePosition(const BWAPI::TilePosition& tilePos);
    std::vector<BWAPI::Unit> GetThreeClosestWorkersToTilePosition(const BWAPI::TilePosition& tilePos, int numselect);
}