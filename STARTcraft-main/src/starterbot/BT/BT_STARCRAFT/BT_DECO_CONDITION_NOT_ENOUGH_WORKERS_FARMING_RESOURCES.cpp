#include "BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES.h"
#include "../DataResources.h"
#include <BWAPI.h>
#include "../../Tools.h"

BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES::BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES(std::string name, BT_NODE* parent)
    : BT_DECO_CONDITION(name, parent, IsThereNotEnoughWorkersFarmingResources) {}


std::string BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES::GetDescription()
{
    return "DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES";
}

bool BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_RESOURCES::IsThereNotEnoughWorkersFarmingResources(void* data)
{
    DataResources* pData = (DataResources*)data;

    std::cout << "Number of mineral miners: " << pData->unitsFarmingMinerals.size() << std::endl;
    std::cout << "Number of gas miners: " << pData->unitsFarmingGas.size() << std::endl << std::endl;

    return (int)pData->unitsFarmingMinerals.size() < pData->nWantedWorkersFarmingMinerals
        && (int)pData->unitsFarmingGas.size() < pData->nWantedWorkersFarmingGas;
}
