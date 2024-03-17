#pragma once

#include <optional>
#include "iostream"
#include "Data.h"
#include "ManagerBase.h"
#include "Data/JobPriorityList.h"
#include "Blackboard.cpp"

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

    //void createPatrolSquad();
    //void sendPatrolSquadToPosition(const BWAPI::Position& targetPosition);

private:
    // Fields
    std::vector<BWAPI::Unitset> squads; // Change this to be a list of class Squad when you've implemented it
    BWAPI::Unitset zealots;

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs;
    /*
    // Fields
    BWAPI::Unitset squads; // Change this to be a list of class Squad when you've implemented it
    contains
        insert
    //int nextSquadId;

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs;*/

};

