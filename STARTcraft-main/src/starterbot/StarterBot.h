#pragma once

#include <vector>
#include "MapTools.h"
#include "BT.h"
#include <BWAPI.h>
#include "Data.h"
#include "Tools.h"
#include "GameFileParser.hpp"
#include "Managers/AuxBaseSupervisor.h"
#include "Managers/BasesManager.h"
#include "Managers/GameCommander.h"
#include "Managers/ScoutManager.h"
#include "Managers/ArmyManager.h"
#include "Blackboard.h"
//#include "map.h"

class StarterBot
{
private:
    MapTools m_mapTools;
	
	GameFileParser gameParser;
	Blackboard blackboard;

	// Managers
	GameCommander gameCommander = GameCommander(blackboard);
	BasesManager basesManager = BasesManager(blackboard);
	BaseSupervisor mainBaseSupervisor = BaseSupervisor(blackboard);
	ScoutManager scoutManager = ScoutManager(blackboard);
	ArmyManager armyManager = ArmyManager(blackboard);
	AuxBaseSupervisor auxBaseSupervisor = AuxBaseSupervisor(blackboard);
	// Fields
	bool alreadySentSupplyJob = false;
	

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
    void drawDebugInformation();

    // functions that are triggered by various BWAPI events from main.cpp
	void onStart();
	void onFrame();
	void onEnd(bool isWinner);
	void onUnitDestroy(BWAPI::Unit unit);
	void onUnitMorph(BWAPI::Unit unit);
	void onSendText(std::string text);
	void onUnitCreate(BWAPI::Unit unit);
	void onUnitComplete(BWAPI::Unit unit);
	void onUnitShow(BWAPI::Unit unit);
	void onUnitHide(BWAPI::Unit unit);
	void onUnitRenegade(BWAPI::Unit unit);
};