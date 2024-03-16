#pragma once
#include <unordered_set>
#include <BWAPI.h>


struct UnitProductionOrder {
	BWAPI::UnitType unitType;
	int orderCount;
	int jobsCount;
	std::unordered_set<BWAPI::Unit> unitInstances;
};

struct SquadProductionOrder {
	std::vector<UnitProductionOrder> productionOrder;
	bool isConstructed;
};


class Blackboard {
public:
	std::unordered_set<BWAPI::Unit> scouts;
	std::vector<BWAPI::Unitset> scout_info;// latest info will be pushed to this vector, all the units the scout sees

	// For ArmyManager to order the production of squads
	std::vector<SquadProductionOrder> squadProductionOrders;
	std::vector<BWAPI::Unitset> squads;
};
