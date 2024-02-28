#pragma once
#include <cassert>
#include "BT_ACTION.h"
#include <chrono>
#include <ctime>  

// Wait a specified amount of time. The task will return running until the task is done waiting. It will return success after the wait time has elapsed.
// 
//--------------------
class BT_ACTION_WAIT : public BT_ACTION {
public:
    BT_ACTION_WAIT(std::string name, BT_NODE* parent,float duration);
protected:
    float Duration; // in seconds
    std::chrono::system_clock::time_point StartTime;
public:
    BT_NODE::State Evaluate(void* data) override;
    std::string GetDescription() override;
};
//----------