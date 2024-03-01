#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"

#include "..\starterbot\BT\Data.h"
#include <format>

#include "..\starterbot\BT\BT.h"
#include "GameFileParser.hpp";



#include "..\starterbot\BT\Data.h"
#include <format>
#include "..\starterbot\BT\BT.h"
#include "GameFileParser.hpp";


StarterBot::StarterBot()
{
    pData = new Data();
    pData->currMinerals = 0;
    pData->thresholdMinerals = THRESHOLD1_MINERALS;
    pData->currSupply = 0;
    pData->thresholdSupply = THRESHOLD1_UNUSED_SUPPLY;
    pData->locationsScouted = 0;
    pData->nWantedWorkersTotal = NWANTED_WORKERS_TOTAL;
    pData->nWantedWorkersFarmingMinerals = NWANTED_WORKERS_FARMING_MINERALS;

    pBtTest = nullptr;
    /*
    //Test BT nodes: BT_DECO_COND_GREATER_THAN, BT_DECO_COND_LESSER_THAN, BT_ACTION_LOG
    pBtTest = new BT_DECORATOR("EntryPoint", nullptr);
    BT_DECO_REPEATER* pForeverRepeater = new BT_DECO_REPEATER("RepeatForever", pBtTest, 0, true, false, false);
    //BT_DECO_COND_GREATER_THAN<int>* pGreaterThan = new BT_DECO_COND_GREATER_THAN<int>("MineralsGreaterThanThreshold1", pForeverRepeater, THRESHOLD1_MINERALS, pData->currMinerals, false);
    //BT_ACTION_LOG* pLog = new BT_ACTION_LOG("LogMSG", pGreaterThan, std::format("current minerals greater than {}", THRESHOLD1_MINERALS));
    BT_DECO_COND_LESSER_THAN<int>* pLesserThan = new BT_DECO_COND_LESSER_THAN<int>("MineralsLesserThanThreshold1", pForeverRepeater, THRESHOLD1_MINERALS, pData->currMinerals, false);
    BT_ACTION_LOG* pLog = new BT_ACTION_LOG("LogMSG", pLesserThan, std::format("current minerals lesser than {}", THRESHOLD1_MINERALS));
    */

    //Test BT nodes: BT_DECO_REPEATER (resetOnRepeat = true), BT_COND_GREATER_THAN, BT_COND_LESSER_THAN, BT_ACTION_LOG
    pBtTest = new BT_DECORATOR("EntryPoint", nullptr);
    BT_DECO_REPEATER* pForeverRepeater = new BT_DECO_REPEATER("RepeatForever", pBtTest, 0, true, false, true);
    BT_SEQUENCER* pSequencer = new BT_SEQUENCER("sequencer",pForeverRepeater, 2);
    //BT_COND_GREATER_THAN<int> *pGreaterThan = new BT_COND_GREATER_THAN<int>("MineralsGreaterThanThreshold1",pSequencer,100, pData->currMinerals, false);
    //BT_ACTION_LOG* pLog = new BT_ACTION_LOG("LogMSG", pSequencer, std::format("current minerals greater than {}", 100));
    BT_COND_LESSER_THAN<int>* pLesserThan = new BT_COND_LESSER_THAN<int>("MineralsLesserThanThreshold1", pSequencer, 100, pData->currMinerals, false);
    //BT_ACTION_LOG* pLog = new BT_ACTION_LOG("LogMSG", pSequencer, std::format("current minerals lesser than {}", 100));



    // Starcraft AI BT
    pBT = new BT_DECORATOR("EntryPoint", nullptr);
    
    BT_PARALLEL_SEQUENCER* pParallelSeq = new BT_PARALLEL_SEQUENCER("MainParallelSequence", pBT, 10);

    //Farming Minerals forever
    BT_DECO_REPEATER* pFarmingMineralsForeverRepeater = new BT_DECO_REPEATER("RepeatForeverFarmingMinerals", pParallelSeq, 0, true, false,false);
    BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS* pNotEnoughWorkersFarmingMinerals = new BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS("NotEnoughWorkersFarmingMinerals", pFarmingMineralsForeverRepeater);
    BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS* pSendWorkerToMinerals = new BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS("SendWorkerToMinerals", pNotEnoughWorkersFarmingMinerals);

    //Training Workers
    //BT_DECO_REPEATER* pTrainingWorkersForeverRepeater = new BT_DECO_REPEATER("RepeatForeverTrainingWorkers", pParallelSeq, 0, true, false,false);
    //BT_DECO_CONDITION_NOT_ENOUGH_WORKERS* pNotEnoughWorkers = new BT_DECO_CONDITION_NOT_ENOUGH_WORKERS("NotEnoughWorkers", pTrainingWorkersForeverRepeater);
    //BT_ACTION_TRAIN_WORKER* pTrainWorker = new BT_ACTION_TRAIN_WORKER("TrainWorker", pNotEnoughWorkers);

    //Build Additional Supply Provider
    BT_DECO_REPEATER* pBuildSupplyProviderForeverRepeater = new BT_DECO_REPEATER("RepeatForeverBuildSupplyProvider", pParallelSeq, 0, true, false,false);
    BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY* pNotEnoughSupply = new BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY("NotEnoughSupply", pBuildSupplyProviderForeverRepeater);
    BT_ACTION_BUILD_SUPPLY_PROVIDER* pBuildSupplyProvider = new BT_ACTION_BUILD_SUPPLY_PROVIDER("BuildSupplyProvider", pNotEnoughSupply);

    // Bases Manager
    basesManager.setChild(&mainBaseSupervisor);

    // Game Commander
    gameCommander.setManagerBases(&basesManager);
    gameCommander.setManagerScout(&scoutManager);
    gameCommander.setManagerArmy(&armyManager);

    // Set early game build order into game commander
    gameParser.parse_game_file("../../src/starterbot/BotParameters/GameFile.json");
    gameCommander.setBuildOrder(gameParser.buildorder);

    gameParser.print_build_order();
}

// Called when the bot starts!
void StarterBot::onStart()
{
    // Set our BWAPI options here    
	BWAPI::Broodwar->setLocalSpeed(10);
    BWAPI::Broodwar->setFrameSkip(0);
    
    // Enable the flag that tells BWAPI top let users enter input while bot plays
    BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

    // Call MapTools OnStart
    m_mapTools.onStart();

    //Bwem
    //BWEM::Map::Instance().Initialize(BWAPI::BroodwarPtr);
}

// Called on each frame of the game
void StarterBot::onFrame()
{
    // Update our MapTools information
    m_mapTools.onFrame();

    pData->currMinerals = BWAPI::Broodwar->self()->minerals();
    pData->currSupply = Tools::GetUnusedSupply(true);

    gameCommander.onFrame();
    
    // AI BT
    if (pBT != nullptr && pBT->Evaluate(pData) != BT_NODE::RUNNING)
    {
        delete (BT_DECORATOR*)pBT;
        pBT = nullptr;
    }

    //Test BT
    if (pBtTest != nullptr && pBtTest->Evaluate(pData) != BT_NODE::RUNNING)
    {
        delete (BT_DECORATOR*)pBtTest;
        pBtTest = nullptr;
    }

    
    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();
    // Send a worker scouting
    sendScout();

    executeAttackStrategy();
    // Draw unit health bars, which brood war unfortunately does not do
    Tools::DrawUnitHealthBars();

    // Draw some relevent information to the screen to help us debug the bot
    drawDebugInformation();
}

// Send our idle workers to mine minerals so they don't just stand there
void StarterBot::sendIdleWorkersToMinerals()
{
    // Let's send all of our starting workers to the closest mineral to them
    // First we need to loop over all of the units that we (BWAPI::Broodwar->self()) own
    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) { unit->rightClick(closestMineral); }
        }
    }
}

// Train more workers so we can gather more income
void StarterBot::trainAdditionalWorkers()
{
    const BWAPI::UnitType workerType = BWAPI::Broodwar->self()->getRace().getWorker();
    const int workersWanted = 20;
    const int workersOwned = Tools::CountUnitsOfType(workerType, BWAPI::Broodwar->self()->getUnits());
    if (workersOwned < workersWanted)
    {
        // get the unit pointer to my depot
        const BWAPI::Unit myDepot = Tools::GetDepot();

        // if we have a valid depot unit and it's currently not training something, train a worker
        // there is no reason for a bot to ever use the unit queueing system, it just wastes resources
        if (myDepot && !myDepot->isTraining()) { myDepot->train(workerType); }
    }
}

// Build more supply if we are going to run out soon
void StarterBot::buildAdditionalSupply()
{
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    if (unusedSupply >= 2) { return; }

    // Otherwise, we are going to build a supply provider
    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    const bool startedBuilding = Tools::BuildBuilding(supplyProviderType);
    if (startedBuilding)
    {
        BWAPI::Broodwar->printf("Started Building %s", supplyProviderType.getName().c_str());
    }
}

void StarterBot::sendScout() {
    const int TotalSupply = Tools::GetTotalSupply(true);

    if (TotalSupply < 5) { return; }

    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();

    // iterate over my units and find a random worker
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker())
        {
            // Find a base on the map
            BWAPI::TilePosition::list StartLocations = BWAPI::Broodwar->getStartLocations();

            // Send the unit scouting
            unit->move((BWAPI::Position)StartLocations[0]);
            break;
        }
    }
}
// TASK:: move to that locations; then for me, propose the priorties ; and make intelligent commands 
void StarterBot::executeAttackStrategy() {
    // First, determine the game phase
    int phase = determineGamePhase();

    // Based on the game phase, decide on the strategy
    this->enemyBaseLocation = findEnemyBasePosition();

    int zealotCount = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits());
    int dragoons = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Dragoon, BWAPI::Broodwar->self()->getUnits());

    int desiredDefenders;

    switch (phase) {
    case 0:
        // Early game strategy   BWAPI::Position enemyBasePosition = findEnemyBasePosition();
        if (this->enemyBaseLocation.isValid())//(!findEnemyBase()) 
        {
            // If enemy base is found, consider harassment or building up forces
            if (shouldHarass()) {
                harassmentStrategy(); // Perform harassment if it's beneficial
            }
            else {
                buildZealotForce(6); // Otherwise, start building up a Zealot force for a rush; Number TBD
                // build buildings
            }
        }
        desiredDefenders = 2;
        if (zealotCount >= 4)//(canRushWithZealots())
        {
            executeZealotRush(); // Execute a Zealot rush if conditions are right
        }
        break;

    case 1://MidGame:
        // Mid game strategy
        

        if (zealotCount >= 4)//(canRushWithZealots())
        {
            executeZealotRush(); // Execute a Zealot rush if conditions are right
        }
        else {
            //continueUnitProduction(); // Keep producing units, transition to mid-game tech
            if (Tools::GetUnusedSupply() > 4) {
                // If we have enough supply, keep training units
                BWAPI::Unit gateway = Tools::GetUnitOfType(BWAPI::UnitTypes::Protoss_Gateway);
                if (gateway && !gateway->isTraining()) {
                    gateway->train(BWAPI::UnitTypes::Protoss_Dragoon); // Or any other unit type
                }
            }
        }
        desiredDefenders = 1;
        break;

    case 2://GamePhase::LateGame:
        // Late game strategy
        //useUnitCombinations(); // Start combining different unit types for a balanced force
        //int zealots = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits());   

        BWAPI::Unit gateway = Tools::GetUnitOfType(BWAPI::UnitTypes::Protoss_Gateway);
        if (gateway && !gateway->isTraining()) {
            if (zealotCount <= dragoons) {
                gateway->train(BWAPI::UnitTypes::Protoss_Zealot);
            }
            else {
                gateway->train(BWAPI::UnitTypes::Protoss_Dragoon);
            }
        }
        desiredDefenders =3;
        break;
    }

    // Common logic for all game phases: attack when you have advantage
    int ourForceSize = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits());
    int enemyForceSize = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->enemy()->getUnits()); // Assuming we're fighting Protoss, what if other cases?
    BWAPI::Unitset defenders;
    BWAPI::Unitset attackers;

    // Sort units into defenders and attackers based on your criteria
    for (auto& unit : myUnits) {
        if (unit->getType().canAttack() && !unit->getType().isWorker()) {
            if (defenders.size() < desiredDefenders) {
                defenders.insert(unit);
            }
            else {
                attackers.insert(unit);
            }
        }
    }

    if (ourForceSize >= enemyForceSize * 2)//(hasArmyAdvantage())
    {
        // Plan the attack using available intelligence

        BWAPI::Position enemyBasePosition = findEnemyBasePosition(); // You need to implement this based on scouting info

        if (this->enemyBaseLocation.isValid())//(!findEnemyBase()) 
        {
            BWAPI::Broodwar->sendText("Planning an attack!");
            for (auto& unit : attackers) {
                if (unit->getType().canAttack() && !unit->isAttacking()) {
                    unit->attack(this->enemyBase);
                }
            }
        }

        // Execute the attack with the available forces
    }
}

// Determines the current phase of the game
int StarterBot::determineGamePhase() {
    int minute = BWAPI::Broodwar->elapsedTime() / 60;

    if (minute < 5) {
        return 0;
    }
    else if (minute >= 5 && minute < 15) {
        return 1;
    }
    else {
        return 2;// GamePhase::Late;
    }
}

// Decides if harassment should be performed based on current game phase and enemy build
//Add
bool StarterBot::shouldHarass() {
    int phase = determineGamePhase();
    // Example condition: we only harass in the early game

    int earlyGameProbeCountThreshold = 7; // Example threshold for early game
    double earlyGameTimeThreshold = 5 * 60; // 5 minutes into the game
    bool isEarlyGame = (phase == 0);//BWAPI::Broodwar->elapsedTime() < earlyGameTimeThreshold;
    bool enoughProbesForEarlyHarass = BWAPI::Broodwar->self()->allUnitCount(BWAPI::UnitTypes::Protoss_Probe) <= earlyGameProbeCountThreshold;

    // Evaluate if we have enough forces to spare a unit for harassment without compromising our economy or defense
    bool economyStable = (pData->nWantedWorkersFarmingMinerals <= pData->unitsFarmingMinerals.size());
    int defenseUnitThreshold = 5; // Example threshold for a "strong" defense
    int defenseUnitsCount = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Photon_Cannon, BWAPI::Broodwar->self()->getUnits());
    bool defenseStable = defenseUnitsCount >= defenseUnitThreshold;
    //bool defenseStable = isDefenseStrong();

    // Evaluate enemy's strength - simple check based on scouting information
    int enemyGatewayCount = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Gateway, BWAPI::Broodwar->enemy()->getUnits());
    int enemyCyberneticsCount = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Cybernetics_Core, BWAPI::Broodwar->enemy()->getUnits());
    bool enemyWeak = enemyGatewayCount == 0 && enemyCyberneticsCount == 0;


    // Decide to harass based on conditions
    bool harass = isEarlyGame && enoughProbesForEarlyHarass && economyStable && defenseStable && enemyWeak;

    return harass;
}

// Builds a force of Zealots
void StarterBot::buildZealotForce(int desiredCount) {
    int currentZealots = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits());

    if (currentZealots < desiredCount) {
        //BWAPI::Unit Tools::GetClosestUnitTo(BWAPI::Unit unit, const BWAPI::Unitset& units)

        BWAPI::Unit gateway = Tools::GetClosestUnitOfType(BWAPI::Broodwar->self(), BWAPI::UnitTypes::Protoss_Gateway);

        if (gateway && !gateway->isTraining()) {
            gateway->train(BWAPI::UnitTypes::Protoss_Zealot);
        }
    }
}

// Executes the Zealot Rush strategy
void StarterBot::executeZealotRush() {
    int zealotCount = Tools::CountUnitsOfType(BWAPI::UnitTypes::Protoss_Zealot, BWAPI::Broodwar->self()->getUnits());
    // Decide on "Hit Early" or "Hit Hard" strategy based on enemy build
    if (zealotCount >= 2 && zealotCount < 6) {
        // Hit Early
        attackWithZealots();
    }
    else if (zealotCount >= 6) {
        // Hit Hard
        attackWithZealots();
    }
}

// Attacks with the current zealot force
void StarterBot::attackWithZealots() {
    BWAPI::Unitset zealots = BWAPI::Broodwar->self()->getUnits();
    for (auto& zealot : zealots) {
        if (zealot->getType() == BWAPI::UnitTypes::Protoss_Zealot) {
            // Find the enemy base location or enemy units
            BWAPI::Position enemyBasePos = findEnemyBasePosition();//!
            if (enemyBasePos.isValid()) {
                // Move the Zealots to enemy base while avoiding being seen
                zealot->attack(this->enemyBase);
            }
        }
    }
}
// which to defend is another question. set up  
// Finds potential enemy base locations (implement scouting logic here)
BWAPI::Position StarterBot::findEnemyBasePosition() {
    // Placeholder for actual scouting logic
    // You would have a list of potential start locations and iterate through them
    // For now, let's just return the first start location
    // Placeholder for the enemy base position, initially set to an invalid position
    static BWAPI::Position enemyBasePosition = BWAPI::Positions::Invalid;

    // If we have already found the enemy base, return its position
    if (enemyBasePosition.isValid())
    {
        return enemyBasePosition;
    }

    // Otherwise, we need to scout and find the enemy base
    for (const auto& unit : BWAPI::Broodwar->enemy()->getUnits())
    {
        // Check if this unit is a resource depot (e.g., Command Center, Nexus, or Hatchery)
        if (unit->getType().isResourceDepot())
        {
            this->enemyBase = unit;
            enemyBasePosition = unit->getPosition();
            return enemyBasePosition;
        }
    }

    // If we haven't found the enemy base yet, return the next potential base location
    // This part assumes that you have some logic to guess or scout for enemy base locations
    BWAPI::TilePosition nextScoutPosition;// = GetNextScoutPosition();
    for (const auto& startLocation : BWAPI::Broodwar->getStartLocations())
    {
        // Ignore already scouted locations
        if (!BWAPI::Broodwar->isExplored(startLocation))
        {
            nextScoutPosition= startLocation;
        }
    }

    // If all start locations have been scouted and no enemy base was found,
    // return an invalid position
    nextScoutPosition= BWAPI::TilePositions::Invalid;
    if (nextScoutPosition.isValid())
    {
        return BWAPI::Position(nextScoutPosition);
    }

    // If no base has been found and there are no more positions to scout,
    // return an invalid position to indicate that the base has not been found
    return BWAPI::Positions::Invalid;
   /* BWAPI::TilePosition::list startLocations = BWAPI::Broodwar->getStartLocations();
    return BWAPI::Position(startLocations.front());*/
}



void StarterBot::harassmentStrategy() {
    // Select a probe for harassment
    BWAPI::Unit harasserProbe= nullptr;// = selectHarassmentProbe();harasserProbe =
    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
        if (unit->getType() == BWAPI::UnitTypes::Protoss_Probe && unit->isIdle()) {
            harasserProbe = unit; // Return the first idle Probe found
        }
    }
    //return nullptr; // No idle Probe available
    if (!harasserProbe) {
        return; // No available probe for harassment
    }

    // Check if we can perform gas stealing
    bool canStealGas = BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Assimilator.mineralPrice() &&
        BWAPI::Broodwar->elapsedTime() < 5 * 60;
    if (canStealGas) {
        BWAPI::Unit enemyGas = Tools::GetClosestUnitOfType(BWAPI::Broodwar->enemy(), BWAPI::UnitTypes::Resource_Vespene_Geyser); //Tools::GetClosestEnemyGas(harasserProbe);
        if (enemyGas && !enemyGas->getType().isRefinery()) {
            // Attempt to build an Assimilator on the enemy's gas
            if (harasserProbe->build(BWAPI::UnitTypes::Protoss_Assimilator, enemyGas->getTilePosition())) {
                return; // Successfully started building an Assimilator, end harassment strategy
            }
        }
    }

    BWAPI::Broodwar->sendText("Cannot steal gas");
    // If not stealing gas, perform hit-and-run tactics
    BWAPI::UnitType workerType;
    
    // Determine the enemy's race and set the worker type accordingly
    BWAPI::Race enemyRace = BWAPI::Broodwar->enemy()->getRace();
    if (enemyRace == BWAPI::Races::Protoss) {
        workerType = BWAPI::UnitTypes::Protoss_Probe;
    }
    else if (enemyRace == BWAPI::Races::Terran) {
        workerType = BWAPI::UnitTypes::Terran_SCV;
    }
    else if (enemyRace == BWAPI::Races::Zerg) {
        workerType = BWAPI::UnitTypes::Zerg_Drone;
    }


    // Use the previously defined GetClosestUnitOfType function
    BWAPI::Unit enemyWorker = Tools::GetClosestUnitOfType(BWAPI::Broodwar->enemy(), workerType);
    //BWAPI::Unit enemyWorker = Tools::GetClosestUnitOfType(BWAPI::Broodwar->enemy(), BWAPI::UnitTypes::Protoss_Probe); //Tools::GetClosestEnemyWorker(harasserProbe);
    if (enemyWorker) {
        // Check if the probe can attack the worker
        if (harasserProbe->getDistance(enemyWorker) < 32) { // Arbitrary attack range
            harasserProbe->attack(enemyWorker);
        }
        else {
            // Move towards the enemy worker to close the distance
            harasserProbe->move(enemyWorker->getPosition());
        }
    }
    else {
        // No enemy workers in sight, move towards enemy mineral line
        BWAPI::Unit enemyMineral = Tools::GetClosestUnitOfType(BWAPI::Broodwar->neutral(), BWAPI::UnitTypes::Resource_Mineral_Field); //Tools::GetClosestEnemyMineral(harasserProbe);
        if (enemyMineral) {
            harasserProbe->move(enemyMineral->getPosition());
        }
    }
}

//BWAPI::Unit StarterBot::selectHarassmentProbe() {
//    for (auto& unit : BWAPI::Broodwar->self()->getUnits()) {
//        if (unit->getType() == BWAPI::UnitTypes::Protoss_Probe && unit->isIdle()) {
//            return unit; // Return the first idle Probe found
//        }
//    }
//    return nullptr; // No idle Probe available
//}

//bool StarterBot::canStealGas(BWAPI::Unit harasserProbe) {
//    // Check if we have enough minerals and it's early in the game
//    return BWAPI::Broodwar->self()->minerals() >= BWAPI::UnitTypes::Protoss_Assimilator.mineralPrice() &&
//        BWAPI::Broodwar->elapsedTime() < 5 * 60; // 5 minutes threshold for early game
//}

// Implement these functions based on your scouting and game map information




// Helper method to perform the actual attack


// ... (other existing methods) ...


// attack: currently naive logic for acting against encountered enemies. //neutralize and destroy logic
void StarterBot::sendAttack()
{
    // Get all of our combat units
    const int TotalSupply = Tools::GetTotalSupply(true);

    if (TotalSupply < 5) { return; }

    BWAPI::Unitset myCombatUnits;
    for (auto& unit : BWAPI::Broodwar->self()->getUnits())
    {
        if (unit->getType().canAttack() && !unit->getType().isWorker() && unit->isCompleted())
        {
            myCombatUnits.insert(unit);
        }
    }

    // Find the closest and weakest enemy unit to attack
    BWAPI::Unit targetEnemy = nullptr;
    int minHp = std::numeric_limits<int>::max();
    double closestDistance = std::numeric_limits<double>::max();
    const BWAPI::Unitset& EnemyUnits = BWAPI::Broodwar->enemy()->getUnits();
    for (auto& unit : EnemyUnits)
    {
        // Ignore the unit if it is a flying building (e.g., Terran Command Center)
        if (unit->getType().isBuilding() && unit->isFlying())
            continue;

    //    // Calculate the distance and hitpoints
        double distance = unit->getPosition().getDistance(myCombatUnits.getPosition());// why units
        int hp = unit->getHitPoints();

    //    // Check if this unit is the closest and weakest
        if (hp < minHp || distance < closestDistance)
        {
            targetEnemy = unit;
            minHp = hp;
            closestDistance = distance;
        }
    }

    //// If we haven't found an enemy, we could either wait or scout for enemies
    //if (!targetEnemy)
    //{
    //    // For now, let's just wait
    //    sendScout();
    //    return;
    //}
    //
    if (targetEnemy) { 
        std::cout << "Enemy found:"<< targetEnemy<<std::endl;
        for (auto& unit : myCombatUnits)
        {
            if (unit->isIdle() || !unit->isAttacking())
            {
                unit->attack(targetEnemy);
            }
        }
    }
    //// Command all of our combat units to attack the found enemy position
    //const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    //for (auto& unit : myUnits)
    //{
    //    if (unit->getType().canAttack() && !unit->getType().isWorker() && unit->isCompleted())
    //    {
    //        if (unit->isIdle() || !unit->isAttacking())
    //        {
    //            unit->attack(targetEnemy);
    //        }
    //    }
    //}
}

double StarterBot::calculatePriorityScore(BWAPI::Unit enemyUnit)
{
    // Example: prioritize by unit type, then by health
    double score = 0.0;

    if (enemyUnit->getType() == BWAPI::UnitTypes::Terran_Siege_Tank_Tank_Mode) {
        score += 100;
    }
    else if (enemyUnit->getType() == BWAPI::UnitTypes::Terran_Medic) {
        score += 80;
    }

    // Prefer lower health units
    score += (1.0 - (enemyUnit->getHitPoints() / enemyUnit->getType().maxHitPoints())) * 50;

    return score;
}
// Draw some relevent information to the screen to help us debug the bot
void StarterBot::drawDebugInformation()
{
    BWAPI::Broodwar->drawTextScreen(BWAPI::Position(10, 10), "Hello, World!\n");
    Tools::DrawUnitCommands();
    Tools::DrawUnitBoundingBoxes();
}

// Called whenever the game ends and tells you if you won or not
void StarterBot::onEnd(bool isWinner)
{
    std::cout << "We " << (isWinner ? "won!" : "lost!") << "\n";
}
// complete these unit class behaviors
// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	//if the unit is farming then remove it from data structure
    if (pData->unitsFarmingMinerals.contains(unit)) pData->unitsFarmingMinerals.erase(unit);
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
    if (unit->getPlayer() == BWAPI::Broodwar->self()) {
        // If the unit belongs to the AI player, you might want to manage it.
        // For example, if it's a Drone morphing into a building, remove it from the worker count.
        if (unit->getType().isBuilding() && pData->unitsFarmingMinerals.contains(unit)) {
            pData->unitsFarmingMinerals.erase(unit);
            // maybe insert
        }//keep track of it; keep track of the existence the units.
    }
}

// Called whenever a text is sent to the game by a user
void StarterBot::onSendText(std::string text) 
{ 
    if (text == "/map")
    {
        m_mapTools.toggleDraw();
    }

}

// Called whenever a unit is created, with a pointer to the destroyed unit
// Units are created in buildings like barracks before they are visible, 
// so this will trigger when you issue the build command for most units
void StarterBot::onUnitCreate(BWAPI::Unit unit)
{ 
    //const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    if (unit->getType().isWorker() && unit->getPlayer() == BWAPI::Broodwar->self()) {
        // Do something when your bot creates a unit. For example, add it to a list of units.
        pData->unitsFarmingMinerals.insert(unit);
        BWAPI::Broodwar->sendText("Created a %s", unit->getType().c_str());
    }
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
    if (unit->getType().supplyProvided() && unit->getPlayer() == BWAPI::Broodwar->self())
    {
        pData->currSupply += unit->getType().supplyProvided();
    }
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
    // If the unit belongs to us, add it to our set of units
    if (unit->getPlayer() == BWAPI::Broodwar->self())
    {
        myUnits.insert(unit);
    }
    // Otherwise, if it's an enemy unit, add it to the enemy units set
    else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        enemyUnits.insert(unit);
    }
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
    // If the unit belongs to us, remove it from our set of units
    if (unit->getPlayer() == BWAPI::Broodwar->self())
    {
        myUnits.erase(unit);
    }
    // Otherwise, if it's an enemy unit, remove it from the enemy units set
    else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        enemyUnits.erase(unit);
    }
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
    myUnits.erase(unit);
    enemyUnits.erase(unit);

    // Now, add it back to the appropriate set based on its new allegiance
    if (unit->getPlayer() == BWAPI::Broodwar->self())
    {
        myUnits.insert(unit);
    }
    else if (unit->getPlayer()->isEnemy(BWAPI::Broodwar->self()))
    {
        enemyUnits.insert(unit);
    }
}