#pragma once

#include "Data.h"
#include "JobBase.h"

class BuildJob : public JobBase {
public:
    BuildJob(int priority, ManagerType assignedManager, int gas, int crystal, BWAPI::UnitType type)
        : JobBase(priority, assignedManager), requiredGas(gas), requiredCrystal(crystal), buildingType(type) {}

    // Getters for the additional fields
    int getRequiredGas() const { return requiredGas; }
    int getRequiredCrystal() const { return requiredCrystal; }
    BWAPI::UnitType getBuildingType() const { return buildingType; }

    bool operator()(const BuildJob& job1, const BuildJob& job2) const {
        // Compare based on priority
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority(); // Higher priority should come first
        }
        // If priorities are equal, compare based on requiredGas
        if (job1.getRequiredGas() != job2.getRequiredGas()) {
            return job1.getRequiredGas() < job2.getRequiredGas(); // Lower requiredGas should come first
        }
        // If requiredGas are equal, compare based on requiredCrystal
        if (job1.getRequiredCrystal() != job2.getRequiredCrystal()) {
            return job1.getRequiredCrystal() < job2.getRequiredCrystal(); // Lower requiredCrystal should come first
        }
        // If requiredCrystal are equal, compare based on buildingType
        // Here you need to define how to compare BWAPI::UnitType objects
        // For example, you can compare their IDs
        return job1.getBuildingType().getID() < job2.getBuildingType().getID();
    }

    BuildJob& operator=(const BuildJob& other) {
        if (this != &other) {
            // Copy members from other to this
            JobBase::operator=(other); // Assign JobBase members
            requiredGas = other.requiredGas;
            requiredCrystal = other.requiredCrystal;
            buildingType = other.buildingType;
        }
        return *this;
    }

private:
    int requiredGas;
    int requiredCrystal;
    BWAPI::UnitType buildingType;
};

struct BuildJobComparator {
    bool operator()(const BuildJob& job1, const BuildJob& job2) const {
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority();
        }
    }
};

using BuildJobPriorityQueue = std::priority_queue<BuildJob, std::vector<BuildJob>, BuildJobComparator>;
