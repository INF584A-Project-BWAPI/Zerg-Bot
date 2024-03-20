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
        gameParser.parse_game_file("GameFile.json");
    };

    // Setters
    void setManagerBases(BasesManager* manager) noexcept { basesManager = manager; };
    void setManagerScout(ScoutManager* manager) noexcept { scoutManager = manager; };
    void setManagerArmy(ArmyManager* manager) noexcept { armyManager = manager; };
    void setBuildOrder(std::vector<BuildingRecipe> buildOrder); // Looks at the build order in the json and distributes the necessary jobs

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
    int attack_threshold_delta = 3000; // Scout might die and will almost alway underestimate enemy strenght, this is to account for that

    std::vector<JobBase> queuedJobs;

    // Functions
    void onFrameChildren();
    void distributeJobs();

    // Based on the data from the blackboard make a decision about if it is time to attack, defend or do nothing.
    void evaluateGameStatus();

    // Used to estimate the strength of the enemy and our strength such that we can determine when to attack.
    void updateOurAttackHitpoints();
    void updateEnemyAttackHitpoints();
    
    void verifySquadStatuses(); // Verify alive squad units and update the unitsets if they are dead.
    void verifySquadOrderStatus(); // Verifies if a squadron is done producing and we can make it an actual unitset to play with.
    void addSquadOrders(); // Adds squad orders until we attack, such that we produce a significant army
    
    void print(std::string order, std::string msg);
};
