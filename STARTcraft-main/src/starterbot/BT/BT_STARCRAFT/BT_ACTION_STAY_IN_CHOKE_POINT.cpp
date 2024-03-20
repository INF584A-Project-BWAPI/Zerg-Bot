#include "BT_ACTION_STAY_IN_CHOKE_POINT.h"
#include "Blackboard.h"

BT_ACTION_STAY_IN_CHOKE_POINT::BT_ACTION_STAY_IN_CHOKE_POINT(std::string name, BT_NODE* parent)
    : BT_ACTION(name, parent) {}

BT_NODE::State BT_ACTION_STAY_IN_CHOKE_POINT::Evaluate(void* data)
{
    return ReturnState(StayInChokePoint(data));
}

std::string BT_ACTION_STAY_IN_CHOKE_POINT::GetDescription()
{
    return "Stays in the choke point and waits for the enemy.";
}


BT_NODE::State BT_ACTION_STAY_IN_CHOKE_POINT::StayInChokePoint(void* data)
{
    std::cout << "In StayInChokePoint function\n";
    Blackboard* pData = (Blackboard*)data;

    // make sure the squads remain around the choke point
    for (const BWAPI::Unitset& squad : pData->squads) {
        squad.move(pData->baseChokePoint);
    }

    return BT_NODE::SUCCESS;
}