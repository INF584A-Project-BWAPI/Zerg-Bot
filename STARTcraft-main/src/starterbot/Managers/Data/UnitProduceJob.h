#pragma once

#include "Data.h"
#include "JobBase.h"

class UnitProduceJob : public JobBase {
public:
    UnitProduceJob(int priority, ManagerType assignedManager, BWAPI::UnitType type)
        : JobBase(priority, assignedManager), unitType(type) {}

    // Getters for the additional fields
    BWAPI::UnitType getUnitType() const { return unitType; }

    bool operator()(const UnitProduceJob& job1, const UnitProduceJob& job2) const {
        // Compare based on priority
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority(); // Higher priority should come first
        }
        // If requiredCrystal are equal, compare based on buildingType
        // Here you need to define how to compare BWAPI::UnitType objects
        // For example, you can compare their IDs
        return job1.getUnitType().getID() < job2.getUnitType().getID();
    }

    UnitProduceJob& operator=(const UnitProduceJob& other) {
        if (this != &other) {
            // Copy members from other to this
            JobBase::operator=(other); // Assign JobBase members
            unitType = other.unitType;
        }
        return *this;
    }

private:
    BWAPI::UnitType unitType;
};

struct UnitProduceJobComparator {
    bool operator()(const UnitProduceJob& job1, const UnitProduceJob& job2) const {
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority();
        }
    }
};

using UnitProduceJobPriorityQueue = std::priority_queue<UnitProduceJob, std::vector<UnitProduceJob>, UnitProduceJobComparator>;
