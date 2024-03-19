#pragma once

#include <vector>;
#include "Data/JobBase.h";
#include "ManagerBase.h";
#include "ScoutManager.h";
#include "ArmyManager.h";
#include "../../starterbot/GameFileParser.hpp";
#include <vector>
#include "Data/JobBase.h"
#include "ManagerBase.h"
#include "ScoutManager.h"
#include "ArmyManager.h"
#include "../Blackboard.h"

class GameCommander : public ManagerBase {
public:
    // Constructor
    GameCommander(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::GameCommander, blackboard) {};

    // Setters
    void setManagerBases(BasesManager* manager) noexcept { basesManager = manager; };
    void setManagerScout(ScoutManager* manager) noexcept { scoutManager = manager; };
    void setManagerArmy(ArmyManager* manager) noexcept { armyManager = manager; };
    void setBuildOrder(std::vector<BuildingRecipe> buildOrder);

    // Functions
    void onFrame();
    void postJob(JobBase job) { queuedJobs.push_back(job); };

private:
    GameFileParser gameParser;

    // Children
    BasesManager* basesManager;
    ScoutManager* scoutManager;
    ArmyManager* armyManager;

    // Fields
    int allocated_minerals = 0;
    int allocated_gas = 0;

    std::vector<JobBase> queuedJobs;

    // Functions
    void onFrameChildren();
    void distributeJobs();
    void newBase();
};
