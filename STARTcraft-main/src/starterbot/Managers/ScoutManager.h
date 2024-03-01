#pragma once

#include "ManagerBase.h"
#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"


class ScoutManager : virtual ManagerBase {
public:
    // Constructor
    ScoutManager() noexcept : ManagerBase(ManagerType::ScoutManager) {};

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
    std::vector<BWAPI::Unit> scouts;

    JobPriorityQueue queuedJobs;
    std::vector<JobBase> activeJobs;

};


