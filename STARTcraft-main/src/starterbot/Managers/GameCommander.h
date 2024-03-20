#pragma once

#include <vector>
#include "Data/JobBase.h"
#include "ManagerBase.h"
#include "ScoutManager.h"
#include "ArmyManager.h"
#include "GameFileParser.hpp"
#include "Blackboard.h"

class GameCommander : public ManagerBase {
public:
    // Constructor
    GameCommander(Blackboard& blackboard) noexcept : ManagerBase(ManagerType::GameCommander, blackboard) {
        gameParser.parse_game_file("../../src/starterbot/BotParameters/GameFile.json");
    };

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

    int attack_threshold_delta = 3000;

    std::vector<JobBase> queuedJobs;

    // Functions
    void onFrameChildren();
    void distributeJobs();
    void evaluateGameStatus();

    void updateOurAttackHitpoints();
    void updateEnemyAttackHitpoints();
    
    void verifySquadStatuses(); // Verify alive squad units and update the unitsets if they are dead.
    void verifySquadOrderStatus(); // Verifies if a squadron is done producing and we can make it an actual unitset to play with.

    void addSquadOrders(); // Adds squad orders until we attack
};
