#pragma once
#include <cassert>
#include "BT_ACTION.h"

//Returns a task status of running
//--------------------
class BT_ACTION_IDLE : public BT_ACTION {
public:
    BT_ACTION_IDLE(std::string name, BT_NODE* parent);
private:
    State Evaluate(void* data) override;    
    std::string GetDescription() override;

    static BT_NODE::State DoNothing(void* data);
};
//----------