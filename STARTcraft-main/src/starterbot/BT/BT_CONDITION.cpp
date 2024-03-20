#include "BT_CONDITION.h"
#include <iostream>

BT_CONDITION::BT_CONDITION(std::string name, BT_NODE* parent, CONDITION_CBK conditionCBK)
    : ConditionCBK(conditionCBK), BT_NODE(name,parent,0) {}

BT_NODE::State BT_CONDITION::Evaluate(void* data) {
    assert(ConditionCBK);
    std::cout << "At " << this->Name << ": " << GetDescription() << "\n";
    if (ConditionCBK(data)) { std::cout << "Condition is returning Success...\n"; return BT_NODE::SUCCESS; }
    else  { std::cout << "Condition is returning Failure...\n"; return Failure(); }
}

std::string BT_CONDITION::GetDescription()
{
    return "CONDITION";
}