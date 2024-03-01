#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"

class BaseSupervisor : virtual ManagerBase {
public:
    // Constructor
    BaseSupervisor() noexcept : ManagerBase(ManagerType::BaseSupervisor) {};
    
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
    std::vector<BWAPI::Unit> workers;
    float defence_dps = 0; // Damage Per Second our defence can provide
    std::vector<BWAPI::UnitType> buildings;

    JobPriorityQueue queuedJobs;
    std::vector<JobBase> activeJobs; // TODO :: change to buildings

    int allocated_minerals = 0;
    int allocated_gas = 0;

    // Functions
    bool buildBuilding(JobBase job);
    bool produceUnit(JobBase job);
    //void verifyActiveBuilds();
};
