#pragma once
#include <unordered_set>
#include <BWAPI.h>

class Blackboard {
public:
	std::unordered_set<BWAPI::Unit> scouts;
	std::vector<BWAPI::Unitset> scout_into;// latest info will be pushed to this vector, all the units the scout sees
};
