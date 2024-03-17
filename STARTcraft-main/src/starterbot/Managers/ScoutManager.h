#pragma once

#include "ManagerBase.h"
#include <optional>
#include "iostream"
#include "../BT/Data.h"
#include "ManagerBase.h"
#include "Data/JobPriorityList.h"
#include "Blackboard.h";


struct scout {
    BWAPI::Unit unit;
    JobBase job = JobBase(0, ManagerType::ScoutManager, JobType::Scouting, false, Importance::High); // default dummy job
    //JobBase job = j;
    bool working = false; // is it scouting actively
    int max_saw = 0;

    void set_working(bool b) { working = b; };
    void set_job(JobBase j) { job = j; };
    bool is_working() const { return working; };
};

class ScoutManager : virtual ManagerBase {
public:
    // Constructor
    ScoutManager(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::ScoutManager, blackboard) {};

    // Functions
    void onFrame();

    // Getters
    const std::vector<JobBase>& getActiveJobs() const noexcept { return activeJobs; };

    // Setters
    void postJob(JobBase job) { queuedJobs.queueTop(job); };

    void makeScout(BWAPI::Unit u);
    void unmakeScout(scout s);
    void checkOnScout(scout * s);
    void sendScouting(scout * s, JobBase job);
    void syncWithBlackboard();

private:

    // Fields
    std::vector<scout> scouts;
    BWAPI::TilePosition::list StartLocations = BWAPI::Broodwar->getStartLocations();
    int ExploredLocations = 0;
    BWAPI::TilePosition HomeLocation = BWAPI::Broodwar->self()->getStartLocation();

    JobPriorityList queuedJobs;
    std::vector<JobBase> activeJobs; // not used

};