#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//Evaluates the specified conditional task. If the conditional task returns success then the child task is run and the child status is returned. If the conditional task does not
// return success then the child task is not run and a failure status is immediately returned.
// 
//--------------------
class BT_DECO_CONDITION : public BT_DECORATOR {
public:
    typedef bool(*CONDITION_CBK)(void* data);
protected:
    CONDITION_CBK  ConditionCBK;

public:
    BT_DECO_CONDITION(std::string name, BT_NODE* parent, CONDITION_CBK conditionCBK);
    State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------