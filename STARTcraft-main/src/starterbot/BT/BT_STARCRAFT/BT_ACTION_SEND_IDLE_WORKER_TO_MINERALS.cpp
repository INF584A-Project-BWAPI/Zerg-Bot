#include "BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS.h"
#include "Tools.h"
#include "Data.h"

BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS::BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS(std::string name,BT_NODE* parent)
    :  BT_ACTION(name,parent) {}

BT_NODE::State BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS::Evaluate(void* data)
{
    return ReturnState(SendIdleWorkerToMinerals(data));
}

std::string BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS::GetDescription()
{
    return "SEND IDLE WORKER TO MINERAL";
}


BT_NODE::State BT_ACTION_SEND_IDLE_WORKER_TO_MINERALS::SendIdleWorkerToMinerals(void* data)
{
    Data* pData = (Data*)data;

    const BWAPI::Unitset& myUnits = BWAPI::Broodwar->self()->getUnits();
    for (auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->getType().isWorker() && unit->isIdle())
        {
            // Get the closest mineral to this worker unit
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) {
                unit->rightClick(closestMineral);
                pData->unitsFarmingMinerals.insert(unit);
                return BT_NODE::SUCCESS;
            }
        }
    }

    return BT_NODE::FAILURE;
}