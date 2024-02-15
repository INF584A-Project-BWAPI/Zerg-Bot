#pragma once
#include <cassert>
#include "BT_NODE.h"

//The sequence task is similar to an AND boolean operation. It will return failure as soon as one of its child tasks return failure
//If a child task returns success then it will sequentially run the next task. If all child tasks return success then it will return success."
//--------------------
class BT_SEQUENCER : public BT_NODE {
private:
    size_t CurrentChildIndex = 0;

public:
    BT_SEQUENCER(std::string name, BT_NODE* parent, size_t childrenMaxCount);
    BT_SEQUENCER(std::string name, size_t childrenMaxCount);

    State Evaluate(void* data) override;
    void Reset() override;
    std::string GetDescription() override;
};
//----------