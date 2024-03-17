#include "BT_ACTION_DEFEND_BASE.h"
#include "Tools.h"
#include "Data.h"

BT_ACTION_DEFEND_BASE::BT_ACTION_DEFEND_BASE(std::string name, BT_NODE* parent)
    : BT_ACTION(name, parent) {}

BT_NODE::State BT_ACTION_DEFEND_BASE::Evaluate(void* data)
{
    return ReturnState(DefendBase(data));
}

std::string BT_ACTION_DEFEND_BASE::GetDescription()
{
    return "Attacks the enemies that are close to the nexus.";
}


BT_NODE::State BT_ACTION_DEFEND_BASE::DefendBase(void* data)
{
    Data* pData = (Data*)data;

    return BT_NODE::SUCCESS;
}