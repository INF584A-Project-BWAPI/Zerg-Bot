#pragma once
#include <unordered_set>
#include <BWAPI.h>

class Blackboard {
public:
	std::unordered_set<BWAPI::Unit> scouts;
};
