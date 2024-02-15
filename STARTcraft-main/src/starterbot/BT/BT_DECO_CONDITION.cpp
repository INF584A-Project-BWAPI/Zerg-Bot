#include "BT_DECO_CONDITION.h"

BT_DECO_CONDITION::BT_DECO_CONDITION(std::string name, BT_NODE* parent, CONDITION_CBK conditionCBK)
    : ConditionCBK(conditionCBK), BT_DECORATOR(name,parent) {}

BT_NODE::State BT_DECO_CONDITION::Evaluate(void* data) {
    assert(ConditionCBK);
    if (ConditionCBK(data))  return ReturnState(BT_DECORATOR::Evaluate(data));
    else return Failure();
}

std::string BT_DECO_CONDITION::GetDescription()
{
    return "DECO_CONDITION";
}