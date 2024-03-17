#pragma once
#include <unordered_set>
#include <set>
#include <BWAPI.h>

enum class GameStatus {
	Standard,
	Defence,
	Attack
};

struct UnitProductionOrder {
	BWAPI::UnitType unitType;
	int orderCount;
	int jobsCount;
	std::unordered_set<BWAPI::Unit> unitInstances;
};

struct SquadProductionOrder {
	std::vector<UnitProductionOrder> productionOrder;
	bool isConstructed;
	std::string name;
};


class Blackboard {
public:
	// Base Nexuses
	std::vector<BWAPI::Unit> baseNexuses;

	// What mode the game is under currently, dictated by the gameCommander
	GameStatus gameStatus = GameStatus::Standard;

	std::unordered_set<BWAPI::Unit> scouts;
	std::vector<BWAPI::Unitset> scout_info;// latest info will be pushed to this vector, all the units the scout sees
	std::unordered_set<BWAPI::Unit> enemyUnits;
	
	int enemyAttackHitpoints = 0;
	int ourAttackHitpoints = 0;

	// For ArmyManager to order the production of squads
	std::vector<SquadProductionOrder> squadProductionOrders;
	std::vector<BWAPI::Unitset> squads;

	// Buildings which produce attackers
	std::set<BWAPI::UnitType> barrackTypes;
};
