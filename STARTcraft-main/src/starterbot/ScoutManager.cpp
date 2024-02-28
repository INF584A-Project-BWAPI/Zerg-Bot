#include "ScoutManager.h"


void ScoutManager::makeScout(BWAPI::Unit& u)
{
	Scouts.push_back(&u);
}

void ScoutManager::unmakeScout(BWAPI::Unit u)
{
	if (std::find(Scouts.begin(), Scouts.end(), u) != Scouts.end()) {
		Scouts.erase(std::find(Scouts.begin(), Scouts.end(), u));
	}
}

void ScoutManager::sendAllScouting()
{
}

void ScoutManager::updateScouts()
{ // iterate over each scout, check if their moving, check what they are seeing, update the BTdata with what they see
}

void ScoutManager::onStart()
{
	StartLocations = BWAPI::Broodwar->getStartLocations();
	Minerals = BWAPI::Broodwar->getMinerals();
	HomeLocation = BWAPI::Broodwar->self()->getStartLocation();
}
