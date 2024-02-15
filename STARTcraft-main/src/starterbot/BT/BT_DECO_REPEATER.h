#pragma once
#include <cassert>
#include "BT_DECORATOR.h"

//The repeater task will repeat execution of its child task until the child task has been run a specified number of times.
//It has the option of continuing to execute the child task even if the child task returns a failure.
// 
//--------------------
class BT_DECO_REPEATER : public BT_DECORATOR {
protected:
    size_t NMaxRepeats;
    bool RepeatForever;
    bool EndOnFailure;
    bool ResetChildOnRepeat;
private:
    size_t NRepeats=0;
public:
    BT_DECO_REPEATER(std::string name, BT_NODE* parent,size_t nMaxRepeats,bool repeatForever,bool endOnFailure,bool resetChildOnRepeat);
    State Evaluate(void* data) override;
    void Reset() override;
    std::string GetDescription() override;
};
//----------