#include "BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS.h"
#include "Data.h"
#include <BWAPI.h>
#include "Tools.h"

BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS::BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS(std::string name, BT_NODE* parent)
    : BT_DECO_CONDITION(name,parent, IsThereNotEnoughWorkersFarmingMinerals) {}


std::string BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS::GetDescription()
{
    return "DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS";
}

bool BT_DECO_CONDITION_NOT_ENOUGH_WORKERS_FARMING_MINERALS::IsThereNotEnoughWorkersFarmingMinerals(void *data)
{
    Data* pData = (Data*)data;
    
    return (int)pData->unitsFarmingMinerals.size() <pData->nWantedWorkersFarmingMinerals;
}
