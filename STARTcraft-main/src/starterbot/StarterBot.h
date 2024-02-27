#pragma once

#include "MapTools.h"
<<<<<<< Updated upstream
#include "..\starterbot\BT\BT.h"
#include <BWAPI.h>
#include "..\starterbot\BT\Data.h"
#include "GameFileParser.hpp";
=======
#include "../starterbot/BT/BT.h"
#include <BWAPI.h>
#include "../starterbot/BT/Data.h"
>>>>>>> Stashed changes
//#include "map.h"

class StarterBot
{
private:
    MapTools m_mapTools;
	BT_NODE* pBT;
	BT_NODE* pBtTest;
	Data *pData;
<<<<<<< Updated upstream
	GameFileParser gameParser;
=======
	BWAPI::Unitset myUnits;
	BWAPI::Unitset enemyUnits;
>>>>>>> Stashed changes

public:

    StarterBot();

    // helper functions to get you started with bot programming and learn the API
    void sendIdleWorkersToMinerals();
    void trainAdditionalWorkers();
    void buildAdditionalSupply();
	void sendScout();
	void sendAttack();
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