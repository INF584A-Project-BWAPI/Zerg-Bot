#include <queue>
#include <vector>
#include "Data.h"

#pragma once

enum class JobType {
    Building,
    UnitProduction
};

enum class ManagerType {
    GameCommander,
    ScoutManager,
    ArmyManager,
    ProductionManager,
    BasesManager,
    BaseSupervisor
};

class JobBase {
public:
    JobBase(int priority, ManagerType assignedManager, JobType jobType)
        : priority(priority), assignedManager(assignedManager), jobType(jobType) {};

    // Setters
    void setPriority(int level) noexcept { priority = level; };
    void setGasCost(int cost) noexcept { gasCost = cost; };
    void setMineralCost(int cost) noexcept { mineralCost = cost; };
    void setSupplyCost(int cost) noexcept { supplyCost = cost; };
    void setUnitType(BWAPI::UnitType type) noexcept { unit = type; };

    // Getters
    int getPriority() const noexcept { return priority; };
    ManagerType getAssignedManager() const noexcept { return assignedManager; }
    JobType getJobType() const noexcept { return jobType; }
    int getGasCost() const noexcept { return gasCost; };
    int getMineralCost() const noexcept { return mineralCost; };
    BWAPI::UnitType getUnit() const noexcept { return unit; };


    // Operator functions used by Heap
    bool operator()(const JobBase& job1, const JobBase& job2) const {
        // Compare based on priority
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority(); // Higher priority should come first
        }
        return job1.getUnit().getID() < job2.getUnit().getID();
    }


private:
    int priority;
    ManagerType assignedManager;
    JobType jobType;

    // Fields used in construction/production jobs
    int gasCost = 0;
    int mineralCost = 0;
    int supplyCost = 0;
    BWAPI::UnitType unit;
};

struct JobComparator {
    bool operator()(const JobBase& job1, const JobBase& job2) const noexcept {
        // Compare based on priority
        if (job1.getPriority() != job2.getPriority()) {
            return job1.getPriority() > job2.getPriority(); // Higher priority should come first
        }
        // If priorities are equal, compare by assigned manager
        return job1.getAssignedManager() > job2.getAssignedManager();
    }
};

using JobPriorityQueue = std::priority_queue<JobBase, std::vector<JobBase>, JobComparator>;