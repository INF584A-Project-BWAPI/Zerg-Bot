#pragma once
#include <unordered_set>
#include <BWAPI.h>

#define NWANTED_WORKERS_TOTAL 20
#define NWANTED_WORKERS_FARMING_MINERALS 10

class DataResources {
public:
	int currMinerals;
	int thresholdMinerals;

	int nWantedWorkersTotal;
	int nWantedWorkersFarmingMinerals;


	std::unordered_set<BWAPI::Unit> unitsFarmingMinerals;
	std::unordered_set<BWAPI::Unit> unitsAvailable;
};
