#include "BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES.h"

#include "Tools.h"
#include "DataResources.h"
#include <set>

BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES::BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES(std::string name, BT_NODE* parent)
    : BT_ACTION(name, parent) {}

BT_NODE::State BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES::Evaluate(void* data)
{
    return ReturnState(SendIdleWorkerToResources(data));
}

std::string BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES::GetDescription()
{
    return "SEND IDLE WORKER TO MINERAL";
}


BT_NODE::State BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES::SendIdleWorkerToResources(void* data)
{
    DataResources* pData = (DataResources*)data;
    const std::unordered_set<BWAPI::Unit>& myUnits = pData->unitsAvailable;

    for (const auto& unit : myUnits)
    {
        // Check the unit type, if it is an idle worker, then we want to send it somewhere
        if (unit->isIdle())
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