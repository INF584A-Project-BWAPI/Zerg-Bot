#include "BT_CONDITION.h"

BT_CONDITION::BT_CONDITION(std::string name, BT_NODE* parent, CONDITION_CBK conditionCBK)
    : ConditionCBK(conditionCBK), BT_NODE(name,parent,0) {}

BT_NODE::State BT_CONDITION::Evaluate(void* data) {
    assert(ConditionCBK);
    if (ConditionCBK(data))  return Success();
    else return Failure();
}

std::string BT_CONDITION::GetDescription()
{
    return "CONDITION";
}