#pragma once
#include "../../BT/Data.h"


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
		: unit(unit) {
	if (unit->isGatheringGas()) {
        status = WorkerStatus::GatheringGas;
    } else if (unit->isGatheringMinerals()) {
        status = WorkerStatus::GatheringMineral;
    } else {
        status = WorkerStatus::Idle;
    }
	};

	// Fields
	BWAPI::Unit& unit;
	WorkerStatus status = WorkerStatus::Unkown;
};
