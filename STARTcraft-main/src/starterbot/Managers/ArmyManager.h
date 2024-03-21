#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include "Data/JobPriorityList.h"
#include "Blackboard.cpp"
#include "AttackGroundUnitBT.h"

class ArmyManager : virtual ManagerBase {
public:
    // Constructor
    ArmyManager(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::ScoutManager, blackboard) {
        attackGroundUnitBT = nullptr;
    };

    // Functions
    void onFrame();

    // Getters
    const std::vector<JobBase>& getActiveJobs() const noexcept { return activeJobs; };

    // Setters
    void postJob(JobBase job) { queuedJobs.queueTop(job); };

private:
    // Fields
    std::vector<BWAPI::Unitset> squads;
    BWAPI::Unitset zealots;

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs;
    AttackGroundUnitBT* attackGroundUnitBT;
};

