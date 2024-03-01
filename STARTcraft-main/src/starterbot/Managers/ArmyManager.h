#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"

class ArmyManager : virtual ManagerBase {
public:
    // Constructor
    ArmyManager() noexcept : ManagerBase(ManagerType::ScoutManager) {};

    // Functions
    void onFrame();

    // Getters
    const JobPriorityQueue& getQueuedJobs() const noexcept { return queuedJobs; };
    const std::vector<JobBase>& getActiveJobs() const noexcept { return activeJobs; };

    std::optional<JobBase> peekTopQueuedJob() noexcept {
        if (queuedJobs.empty())
            return std::nullopt; // Return an empty optional to indicate an empty state

        return queuedJobs.top();
    }

    // Setters
    void postJob(JobBase job);
    void postActiveJob(JobBase job) { activeJobs.push_back(job); };

    void popTopJob() { queuedJobs.pop(); };

private:
    // Fields
    std::vector<BWAPI::Unit> squads; // Change this to be a list of class Squad when you've implemented it

    JobPriorityQueue queuedJobs;
    std::vector<JobBase> activeJobs;

};

