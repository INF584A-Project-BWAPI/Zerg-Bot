#include "BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY.h"
#include "Data.h"
#include <BWAPI.h>
#include "Tools.h"

BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY::BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY(std::string name, BT_NODE* parent)
    : BT_DECO_CONDITION(name,parent, IsThereNotEnoughSupply) {}


std::string BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY::GetDescription()
{
    return "DECO_CONDITION_NOT_ENOUGH_SUPPLY";
}

bool BT_DECO_CONDITION_NOT_ENOUGH_SUPPLY::IsThereNotEnoughSupply(void *data)
{
    Data* pData = (Data*)data;
    
    // Get the amount of supply supply we currently have unused
    const int unusedSupply = Tools::GetTotalSupply(true) - BWAPI::Broodwar->self()->supplyUsed();

    // If we have a sufficient amount of supply, we don't need to do anything
    return unusedSupply < pData->thresholdSupply;

}
