#pragma once

#include "ManagerBase.h"
#include <optional>
#include "iostream"
#include "../BT/Data.h"

#include "ManagerBase.h"
#include "Data/JobPriorityList.h"


class ScoutManager : virtual ManagerBase {
public:
    // Constructor
    ScoutManager() noexcept : ManagerBase(ManagerType::ScoutManager) {};

    // Functions
    void onFrame();

    // Getters
    const std::vector<JobBase>& getActiveJobs() const noexcept { return activeJobs; };

    // Setters
    void postJob(JobBase job) { queuedJobs.queueTop(job); };

private:
    // Fields
    std::vector<BWAPI::Unit> scouts;

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs;

};


