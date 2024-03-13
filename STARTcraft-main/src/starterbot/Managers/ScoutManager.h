#pragma once

#include "ManagerBase.h"
#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include "Data/JobPriorityList.h"

struct scout {
    BWAPI::Unit* unit;
    JobBase job;
    bool working = false;
};

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
    std::vector<scout> scouts;
    BWAPI::TilePosition::list StartLocations = BWAPI::Broodwar->getStartLocations();
    int ExploredLocations = 0;
    BWAPI::TilePosition HomeLocation = BWAPI::Broodwar->self()->getStartLocation();

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs; // not used

};