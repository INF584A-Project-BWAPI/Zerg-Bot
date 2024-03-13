#pragma once

#include "Data.h"

enum class WorkerStatus {
	GatheringMineral,
	GatheringGas,
	Building,
	Idle,
	Unkown
};


class Worker {

public:
	Worker(BWAPI::Unit& unit)
		: unit(unit) {};

	// Fields
	BWAPI::Unit& unit;
	WorkerStatus status = WorkerStatus::Unkown;
};
