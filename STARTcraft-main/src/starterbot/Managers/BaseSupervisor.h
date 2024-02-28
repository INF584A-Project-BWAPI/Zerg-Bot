#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include "Data/BuildJob.h"
#include "Data/UnitProduceJob.h"

class BaseSupervisor : public ManagerBase {
public:
    // Constructor
    BaseSupervisor() : ManagerBase() {};
    
    // Functions
    void onFrame();

    const BuildJobPriorityQueue& getQueuedBuildJobs() const {
        return queuedBuildJobs;
    }

    const UnitProduceJobPriorityQueue& getQueuedUnitProduceJobs() const {
        return queuedUnitProduceJobs;
    }


    const std::vector<BuildJob>& getActiveBuildJobs() const {
        return activeBuildJobs;
    }

    const std::vector<UnitProduceJob>& getActiveUnitProduceJobs() const {
        return activeUnitProduceJobs;
    }

    void postActiveJob(BuildJob job) {
        activeBuildJobs.push_back(job);
    }

    void postActiveJob(UnitProduceJob job) {
        activeUnitProduceJobs.push_back(job);
    }

    void removeTopBuildJob() {
        queuedBuildJobs.pop();
    }

    void removeTopUnitProduceJob() {
        queuedUnitProduceJobs.pop();
    }

    std::optional<BuildJob> peekBuildJob() {
        if (queuedBuildJobs.empty())
            return std::nullopt; // Return an empty optional to indicate an empty state

        return queuedBuildJobs.top();
    }

    std::optional<UnitProduceJob> peekUnitProduceJob() {
        if (queuedUnitProduceJobs.empty())
            return std::nullopt; // Return an empty optional to indicate an empty state

        return queuedUnitProduceJobs.top();
    }

    void postJob(BuildJob job);
    void postJob(UnitProduceJob job);
    BuildJobPriorityQueue getManagerBuildJobs(ManagerType assignedManager);
    BuildJobPriorityQueue getManagerUnitProduceJobs(ManagerType assignedManager);

private:
    // Fields
    std::vector<BWAPI::Unit> workers;
    float defence_dps; // Damage Per Second our defence can provide
    std::vector<BWAPI::UnitType> buildings;

    BuildJobPriorityQueue queuedBuildJobs;
    std::vector<BuildJob> activeBuildJobs;

    UnitProduceJobPriorityQueue queuedUnitProduceJobs;
    std::vector<UnitProduceJob> activeUnitProduceJobs;


    int allocated_crystal = 0;
    int allocated_gas = 0;

    // Functions
    bool buildBuilding(BuildJob job);
    bool produceUnit(UnitProduceJob job);
    void verifyActiveBuilds();
};
