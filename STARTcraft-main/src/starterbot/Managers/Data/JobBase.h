#include <queue>
#include <vector>
#include "Data.h"

#pragma once

enum class Importance {
    High,
    Low
};

enum class JobType {
    Building,
    UnitProduction,
    Scouting
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
    JobBase(int priority, ManagerType assignedManager, JobType jobType, bool blocking, Importance importance)
        : priority(priority)
        , assignedManager(assignedManager)
        , jobType(jobType)
        , blocking(blocking)
        , importance(importance) {};

    // Fields 
    bool blocking;
    Importance importance;


    // Setters
    void setPriority(int level) noexcept { priority = level; };
    void setGasCost(int cost) noexcept { gasCost = cost; };
    void setMineralCost(int cost) noexcept { mineralCost = cost; };
    void setSupplyCost(int cost) noexcept { supplyCost = cost; };
    void setUnitType(BWAPI::UnitType type) noexcept { unit = type; };
    void setTargetLocation(BWAPI::TilePosition l) noexcept { targetLocation = l; };

    // Getters
    int getPriority() const noexcept { return priority; };
    ManagerType getAssignedManager() const noexcept { return assignedManager; }
    JobType getJobType() const noexcept { return jobType; }
    int getGasCost() const noexcept { return gasCost; };
    int getMineralCost() const noexcept { return mineralCost; };
    BWAPI::UnitType getUnit() const noexcept { return unit; };
    BWAPI::TilePosition getTargetLocation() const noexcept { return targetLocation; };

    // Used to compare jobs in the job priority queue
    bool operator < (const JobBase& j) const noexcept {
        return priority < j.getPriority();
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

    // Fields used in scouting jobs
    BWAPI::TilePosition targetLocation;// = BWAPI::Broodwar->self()->getStartLocation();
};
