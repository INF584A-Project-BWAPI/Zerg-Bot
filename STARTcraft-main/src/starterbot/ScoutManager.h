#pragma once

#include "MapTools.h"
#include "BT.h"
#include <BWAPI.h>
#include "Data.h"

class ScoutManager
{
private:
    BWAPI::TilePosition::list StartLocations;
    BWAPI::Unitset Minerals;
    BWAPI::TilePosition HomeLocation;
    std::vector<BWAPI::Unit*> Scouts;

public:
    void makeScout(BWAPI::Unit& u);
    void unmakeScout(BWAPI::Unit u);
    void sendAllScouting();
    void onStart();
    void updateScouts();
};

