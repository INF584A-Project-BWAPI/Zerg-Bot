#include "BT_ACTION_IDLE.h"

BT_ACTION_IDLE::BT_ACTION_IDLE(std::string name,BT_NODE* parent)
    : BT_ACTION(name,parent) {}

BT_NODE::State BT_ACTION_IDLE::Evaluate(void* data)
{
    return ReturnState(DoNothing(data));
}


std::string BT_ACTION_IDLE::GetDescription()
{
    return "IDLE ACTION";
}


BT_NODE::State BT_ACTION_IDLE::DoNothing(void* data)
{
    return RUNNING;
}
