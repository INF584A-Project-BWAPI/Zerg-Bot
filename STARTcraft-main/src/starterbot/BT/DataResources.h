#pragma once
#include <unordered_set>
#include <BWAPI.h>

class DataResources {
public:
	int nWantedWorkersFarmingMinerals;
	int nWantedWorkersFarmingGas;

	bool assimilatorAvailable;
	BWAPI::Unit assimilatorUnit;
	BWAPI::Unit nexus;

	std::unordered_set<BWAPI::Unit> unitsFarmingMinerals;
	std::unordered_set<BWAPI::Unit> unitsFarmingGas;
};
