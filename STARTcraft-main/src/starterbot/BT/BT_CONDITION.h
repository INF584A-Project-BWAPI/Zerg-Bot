#pragma once
#include <cassert>
#include "BT_NODE.h"

//Evaluates the specified conditional task. If the conditional task returns true then return success, else return failure
// no running state 
//--------------------
class BT_CONDITION : public BT_NODE {
public:
    typedef bool(*CONDITION_CBK)(void* data);
protected:
    CONDITION_CBK  ConditionCBK;

public:
    BT_CONDITION(std::string name, BT_NODE* parent, CONDITION_CBK conditionCBK);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------