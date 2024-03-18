#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <Data.h>
#include <format>

#include "BT.h"
#include "Managers/GameCommander.h";
#include "GameFileParser.hpp";


StarterBot::StarterBot()
{
    // Add barrack types to the blackboard
    blackboard.barrackTypes.insert(BWAPI::UnitTypes::Protoss_Gateway);
    blackboard.barrackTypes.insert(BWAPI::UnitTypes::Protoss_Robotics_Facility);
    blackboard.barrackTypes.insert(BWAPI::UnitTypes::Protoss_Stargate);

    // Add workers to the main base supervisor
    const BWAPI::Unitset myUnits = BWAPI::Broodwar->self()->getUnits();

    for (auto& unit : myUnits) {
        if (unit->getType().isWorker()) {
            mainBaseSupervisor.addWorker(unit);
        }
    }

    // Set the main base supervisor
    basesManager.setChild(&mainBaseSupervisor);

    // Game Commander
    gameCommander.setManagerBases(&basesManager);
    gameCommander.setManagerScout(&scoutManager);
    gameCommander.setManagerArmy(&armyManager);

    // Set early game build order into game commander
    gameParser.parse_game_file("../../src/starterbot/BotParameters/GameFile.json");
    gameCommander.setBuildOrder(gameParser.buildorder);
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
    gameCommander.onFrame();
    


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
    if (unusedSupply > 2 || alreadySentSupplyJob) {
        alreadySentSupplyJob = false;
        return;
    }

    // Otherwise, we are going to build a supply provider
    //BWAPI::Broodwar->printf("Supply is running out (building more): %s", unusedSupply);

    const BWAPI::UnitType supplyProviderType = BWAPI::Broodwar->self()->getRace().getSupplyProvider();

    JobBase job(0, ManagerType::BaseSupervisor, JobType::Building, false, Importance::High);
    job.setUnitType(supplyProviderType);
    job.setGasCost(supplyProviderType.gasPrice());
    job.setMineralCost(supplyProviderType.mineralPrice());

    gameCommander.postJob(job);

    alreadySentSupplyJob = true;
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

// Called whenever a unit is destroyed, with a pointer to the unit
void StarterBot::onUnitDestroy(BWAPI::Unit unit)
{
	//if the unit is farming then remove it from data structure
    if (BWAPI::Broodwar->self()->isEnemy(unit->getPlayer())) {
        if (blackboard.enemyUnits.contains(unit)) {
            blackboard.enemyUnits.erase(unit);
        }
    }
}

// Called whenever a unit is morphed, with a pointer to the unit
// Zerg units morph when they turn into other units
void StarterBot::onUnitMorph(BWAPI::Unit unit)
{
	
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
	
}

// Called whenever a unit finished construction, with a pointer to the unit
void StarterBot::onUnitComplete(BWAPI::Unit unit) {
    if (unit->exists()) {
        for (SquadProductionOrder& squadOrder : blackboard.squadProductionOrders) {
            bool assigned = false;

            if (!squadOrder.isConstructed) {
                for (UnitProductionOrder& unitOrder : squadOrder.productionOrder) {
                    if (unitOrder.unitType == unit->getType()) {
                        unitOrder.unitInstances.insert(unit);
                        assigned = true;
                    }
                }
            }

            if (assigned)
                break;
        }
    }
}

// Called whenever a unit appears, with a pointer to the destroyed unit
// This is usually triggered when units appear from fog of war and become visible
void StarterBot::onUnitShow(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit gets hidden, with a pointer to the destroyed unit
// This is usually triggered when units enter the fog of war and are no longer visible
void StarterBot::onUnitHide(BWAPI::Unit unit)
{ 
	
}

// Called whenever a unit switches player control
// This usually happens when a dark archon takes control of a unit
void StarterBot::onUnitRenegade(BWAPI::Unit unit)
{ 
	
}