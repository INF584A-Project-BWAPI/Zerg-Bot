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
    return "SEND IDLE WORKER TO RESOURCES";
}


BT_NODE::State BT_ACTION_SEND_IDLE_WORKER_TO_RESOURCES::SendIdleWorkerToResources(void* data)
{
    DataResources* pData = (DataResources*)data;
    BT_NODE::State orderSent = BT_NODE::FAILURE;

    for (const auto& unit : pData->unitsFarmingMinerals) {
        if (unit->isIdle()) {
            //if (unit->isCarryingMinerals()) {
            //    unit->rightClick(pData->nexus);
            //    continue;
            //}
            
            BWAPI::Unit closestMineral = Tools::GetClosestUnitTo(unit, BWAPI::Broodwar->getMinerals());

            // If a valid mineral was found, right click it with the unit in order to start harvesting
            if (closestMineral) {
                unit->rightClick(closestMineral);

                return BT_NODE::SUCCESS;
            }
        }
    }

    for (const auto& unit : pData->unitsFarmingGas) {
        if ((unit->isIdle()  || (unit->isGatheringMinerals() && !unit->isCarryingMinerals())) 
            && pData->assimilatorAvailable) {

            unit->rightClick(pData->assimilatorUnit);

            return BT_NODE::SUCCESS;
        }
    }

    return BT_NODE::FAILURE;
}