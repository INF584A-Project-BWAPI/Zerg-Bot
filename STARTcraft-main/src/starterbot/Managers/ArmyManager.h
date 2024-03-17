#pragma once

#include <optional>
#include "iostream"
#include "../BT/Data.h"
#include "ManagerBase.h"
#include "Data/JobPriorityList.h"

class ArmyManager : virtual ManagerBase {
public:
    // Constructor
    ArmyManager(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::ScoutManager, blackboard) {};

    // Functions
    void onFrame();

    // Getters
    const std::vector<JobBase>& getActiveJobs() const noexcept { return activeJobs; };

    // Setters
    void postJob(JobBase job) { queuedJobs.queueTop(job); };

private:
    // Fields
    std::vector<BWAPI::Unit> squads; // Change this to be a list of class Squad when you've implemented it

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs;

};

