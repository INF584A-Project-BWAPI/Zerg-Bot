#pragma once
#include <cassert>
#include "BT_ACTION.h"

// Wait a specified amount of time. The task will return running until the task is done waiting. It will return success after the wait time has elapsed.
// 
//--------------------
class BT_ACTION_LOG : public BT_ACTION {
public:
    BT_ACTION_LOG(std::string name, BT_NODE* parent,std::string msg);
protected:
    std::string Msg;
public:
    BT_NODE::State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------