#include "StarterBot.h"
#include "Tools.h"
#include "MapTools.h"
#include <Data.h>
#include <format>

#include "BT.h"

StarterBot::StarterBot()
{
    pData = new Data();
    pData->currMinerals = 0;
    pData->thresholdMinerals = THRESHOLD1_MINERALS;
    pData->currSupply = 0;
    pData->thresholdSupply = THRESHOLD1_UNUSED_SUPPLY;

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
    BT_ACTION_LOG* pLog = new BT_ACTION_LOG("LogMSG", pSequencer, std::format("current minerals lesser than {}", 100));



    // Starcraft AI BT
    pBT = new BT_DECORATOR("EntryPoint", nullptr);
    
    BT_PARALLEL_SEQUENCER* pParallelSeq = new BT_PARALLEL_SEQUENCER("MainParallelSequence", pBT, 10);

    //Farming Minerals forever
    BT_DECO_REPEATER* pFarmingMineralsForeverRepeater = new BT_DECO_REPEATER("RepeatForeverFarmingMinerals", pParallelSeq, 0, true, false,false);
    BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS* pNotEnoughWorkersFarmingMinerals = new BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS("NotEnoughWorkersFarmingMinerals", pFarmingMineralsForeverRepeater);
    BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS* pSendWorkerToMinerals = new BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS("SendWorkerToMinerals", pNotEnoughWorkersFarmingMinerals);

    //Training Workers
    BT_DECO_REPEATER* pTrainingWorkersForeverRepeater = new BT_DECO_REPEATER("RepeatForeverTrainingWorkers", pParallelSeq, 0, true, false,false);
    BT_DECO_CONDITION_NOT_ENOUGH_WORKERS* pNotEnoughWorkers = new BT_DECO_CONDITION_NOT_ENOUGH_WORKERS("NotEnoughWorkers", pTrainingWorkersForeverRepeater);
    BT_ACTION_TRAIN_WORKER* pTrainWorker = new BT_ACTION_TRAIN_WORKER("TrainWorker", pNotEnoughWorkers);

    //Build Additional Supply Provider
    BT_DECO_REPEATER* pBuildSupplyProviderForeverRepeater = new BT_DECO_REPEATER("RepeatForeverBuildSupplyProvider", pParallelSeq, 0, true, false,false);
    BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY* pNotEnoughSupply = new BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY("NotEnoughSupply", pBuildSupplyProviderForeverRepeater);
    BT_ACTION_BUILD_SUPPLY_PROVIDER* pBuildSupplyProvider = new BT_ACTION_BUILD_SUPPLY_PROVIDER("BuildSupplyProvider", pNotEnoughSupply);
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

    /*
    // Send our idle workers to mine minerals so they don't just stand there
    sendIdleWorkersToMinerals();

    // Train more workers so we can gather more income
    trainAdditionalWorkers();

    // Build more supply if we are going to run out soon
    buildAdditionalSupply();
    */

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
    if (pData->unitsFarmingMinerals.contains(unit)) pData->unitsFarmingMinerals.erase(unit);
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
void StarterBot::onUnitComplete(BWAPI::Unit unit)
{
	
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